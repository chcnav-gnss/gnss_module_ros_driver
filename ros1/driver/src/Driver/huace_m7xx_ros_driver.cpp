#include "huace_m7xx_ros_driver/Driver/huace_m7xx_ros_driver.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_class.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_gmf.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_nmea.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_response.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_outconfig.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_port.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include "huace_m7xx_ros_driver/Driver/Command/config_com.h"
#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <utility>

namespace huace_m7xx_ros_driver
{

    HuaceM7xxDriver::HuaceM7xxDriver(std::shared_ptr<SerialPort> io, uint32_t baudrate)
        : io(io), delay_ms(0), message_classes_initialized(false), separate_read_thread_started(false), drop_byte_count(0)
    {
        Log::Trace("HuaceM7xxDriver Serial Port Ctor.");
        Log::Info("Welcome to HuaceM7xxDriver!");
        this->Init();
        if (Ok != this->AutoSetBaudrate(baudrate)) {
            throw std::runtime_error("HuaceM7xxDriver failed to auto set baudrate to " + std::to_string(baudrate));
        }
        this->StartSeparateReadThread();
    }

    HuaceM7xxDriver::HuaceM7xxDriver(std::shared_ptr<FilePort> io, uint32_t delay_ms)
        : io(io), delay_ms(delay_ms), message_classes_initialized(false), separate_read_thread_started(false), drop_byte_count(0)
    {
        Log::Trace("HuaceM7xxDriver File Port Ctor.");
        Log::Info("Welcome to HuaceM7xxDriver!");
        this->Init();
    }

    HuaceM7xxDriver::~HuaceM7xxDriver() {
        Log::Trace("HuaceM7xxDriver Dtor.");
        this->supported_msg_class.clear();
    }

    void HuaceM7xxDriver::Init(void) {
        this->supported_msg_class.insert({MessageClassGmf::MESSAGE_CLASS_ID, std::make_shared<MessageClassGmf>()});
        this->supported_msg_class.insert({MessageClassNMEA::MESSAGE_CLASS_ID, std::make_shared<MessageClassNMEA>()});
        this->supported_msg_class.insert({MessageClassResponse::MESSAGE_CLASS_ID, std::make_shared<MessageClassResponse>()});
        this->supported_msg_class.insert({MessageClassOutConfig::MESSAGE_CLASS_ID, std::make_shared<MessageClassOutConfig>()});
        this->supported_msg_class.insert({MessageClassPort::MESSAGE_CLASS_ID, std::make_shared<MessageClassPort>()});
        this->message_classes_initialized = true;
    }

    Result HuaceM7xxDriver::AutoSetBaudrate(uint32_t baudrate) {
        int32_t error_code = 1;
        std::array<uint32_t, 11> baudrate_list = {115200, 230400, 460800, 921600, 1500000, 3000000, 4800, 9600, 19200, 38400, 57600};

        /** Setting Baudrate in non-block mode */
        if (Ok != this->io->SetBlockMode(false)) {
            Log::Error("Failed to set non-block mode before auto baudrate.");
            return Err;
        }

        /** Register RESPONSE Callback to extract M7xx response */
        if (Ok != this->RegisterMessageCallback(MessageClass::MESSAGE_CLASS_RESPONSE, message::Response::MESSAGE_ID, "response",
            [](const std::vector<uint8_t> &msg, void *user) -> Result {
                message::Response::ERROR_CODE_E res = message::Response::GetErrorCode(msg);
                *(int32_t *)user = (int32_t)res;
                return Ok;
            }, (void *)&error_code)) {
            Log::Error("Failed to register temporary response callback for auto baudrate.");
            (void)this->io->SetBlockMode();
            return Err;
        }

        for(auto it = baudrate_list.begin(); it != baudrate_list.end(); ++it) {
            /** Set Host Baudrate */
            if (Ok != this->io->SetBaudRate(*it)) {
                Log::Error("Failed to Set BaudRate.");
                if (Ok != this->UnregisterMessageCallback(MessageClass::MESSAGE_CLASS_RESPONSE, message::Response::MESSAGE_ID, "response")) {
                    Log::Error("UnRegisterMessageCallback Failed.");
                }
                (void)this->io->SetBlockMode();
                return Err;
            }

            /** Send CONFIGCOM Command to set M7xx baudrate */
            command::ConfigCom cmd = command::ConfigCom(baudrate);
            if (Ok != this->ExecuteCommand(cmd)) {
                Log::Warn("CONFIGCOM Command execute failed.");
            }

            /**< The baud rate setting behavior cannot be immediately followed by the write behavior. */
            usleep(100 * 1000); // 100ms

            /** Set Host Baudrate */
            if (Ok != this->io->SetBaudRate(baudrate)) {
                Log::Error("Set Host BaudRate failed.");
                if (Ok != this->UnregisterMessageCallback(MessageClass::MESSAGE_CLASS_RESPONSE, message::Response::MESSAGE_ID, "response")) {
                    Log::Error("UnRegisterMessageCallback Failed.");
                }
                (void)this->io->SetBlockMode();
                return Err;
            }

            this->ProcessMessages();

            if (1 == error_code) {
                Log::Warn("CONFIGCOM Command not response.");
            } else if (0 == error_code) {
                Log::Trace("CONFIGCOM Command response Ok.");
                break;
            } else if (0 <= error_code) {
                Log::Warn("CONFIGCOM Command response Err: " + std::to_string(error_code));
                break;
            }
        }

        // unrigister response callback
        if (Ok != this->UnregisterMessageCallback(MessageClass::MESSAGE_CLASS_RESPONSE, message::Response::MESSAGE_ID, "response")) {
            Log::Error("UnRegisterMessageCallback Failed.");
            (void)this->io->SetBlockMode();
            return Err;
        }

        /** Recover to block mode after setting baudrate */
        if (Ok != this->io->SetBlockMode()) {
            Log::Error("Failed to recover block mode after auto baudrate.");
            return Err;
        }

        return 0 == error_code ? Ok : Err;
    }

    Result HuaceM7xxDriver::SendMessage(const uint8_t *msg, uint32_t length) {
        std::lock_guard<std::mutex> lock(this->write_mutex);
        return this->io->Submit(msg, length);
    }

    Result HuaceM7xxDriver::SendMessage(const std::vector<uint8_t> &msg) {
        std::lock_guard<std::mutex> lock(this->write_mutex);
        return this->io->Submit(msg);
    }

    Result HuaceM7xxDriver::SendMessage(const std::shared_ptr<std::vector<uint8_t>> msg) {
        std::lock_guard<std::mutex> lock(this->write_mutex);
        return this->io->Submit(msg);
    }

    Result HuaceM7xxDriver::SendMessage(const std::string &msg) {
        std::lock_guard<std::mutex> lock(this->write_mutex);
        return this->io->Submit(msg);
    }

    Result HuaceM7xxDriver::SendMessage(const std::shared_ptr<std::string> msg) {
        std::lock_guard<std::mutex> lock(this->write_mutex);
        return this->io->Submit(msg);
    }

    Result HuaceM7xxDriver::RequestMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, command::Outmsg::OUTMSG_PERIOD_E period) {
        std::string cmd;
        if (0 == this->supported_msg_class.count(msg_cls)) {
            return Err;
        }
        if (0 == this->supported_msg_class.at(msg_cls)->supported_msgs.count(msg_id)) {
            return Err;
        }
        if (Ok == this->supported_msg_class.at(msg_cls)->supported_msgs.at(msg_id)->GetRequestMessageCmd(period, cmd)) {
            return this->SendMessage(cmd);
        } else {
            return Err;
        }
    }

    Result HuaceM7xxDriver::RequestSlaveAntMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, command::Outmsg::OUTMSG_PERIOD_E period) {
        std::string cmd;
        if (0 == this->supported_msg_class.count(msg_cls)) {
            return Err;
        }
        if (0 == this->supported_msg_class.at(msg_cls)->supported_msgs.count(msg_id)) {
            return Err;
        }
        if (Ok == this->supported_msg_class.at(msg_cls)->supported_msgs.at(msg_id)->GetRequestSlaveAntMessageCmd(period, cmd)) {
            return this->SendMessage(cmd);
        } else {
            return Err;
        }
    }

    Result HuaceM7xxDriver::CancelMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id) {
        std::string cmd;
        if (0 == this->supported_msg_class.count(msg_cls)) {
            return Err;
        }
        if (0 == this->supported_msg_class.at(msg_cls)->supported_msgs.count(msg_id)) {
            return Err;
        }
        if (Ok == this->supported_msg_class.at(msg_cls)->supported_msgs.at(msg_id)->GetCancelMessageCmd(cmd)) {
            return this->SendMessage(cmd);
        } else {
            return Err;
        }
    }

    Result HuaceM7xxDriver::CancelSlaveAntMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id) {
        std::string cmd;
        if (0 == this->supported_msg_class.count(msg_cls)) {
            return Err;
        }
        if (0 == this->supported_msg_class.at(msg_cls)->supported_msgs.count(msg_id)) {
            return Err;
        }
        if (Ok == this->supported_msg_class.at(msg_cls)->supported_msgs.at(msg_id)->GetCancelSlaveAntMessageCmd(cmd)) {
            return this->SendMessage(cmd);
        } else {
            return Err;
        }
    }

    Result HuaceM7xxDriver::ExecuteCommand(command::Command &cmd) {
        std::string cmd_str;
        if (Ok == cmd.GetCmdString(cmd_str)) {
            return this->SendMessage(cmd_str);
        } else {
            return Err;
        }
    }

    Result HuaceM7xxDriver::ExecuteCommand(const std::shared_ptr<command::Command> cmd) {
        if (nullptr == cmd) {
            return Err;
        }
        return this->ExecuteCommand(*cmd);
    }

    Result HuaceM7xxDriver::ExecuteCommand(const std::string &cmd_str) {
        return this->SendMessage(cmd_str);
    }

    Result HuaceM7xxDriver::ExecuteCommand(const std::shared_ptr<std::string> cmd_str) {
        return this->SendMessage(cmd_str);
    }

    void HuaceM7xxDriver::RequestStop(void) {
        this->io->RequestStop();
    }

    void HuaceM7xxDriver::StartSeparateReadThread(void) {
        this->io->SetupSeparateThread();
        this->separate_read_thread_started = true;
    }

    void HuaceM7xxDriver::ProcessMessages(void) {
        message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
        MessageClass::MATCH_RESULT_E mres;
        uint32_t data_size;
        MessageClass::MESSAGE_CLASS_E current_cls_id = MessageClass::MESSAGE_CLASS_MAX;

        /* receive a bulk of data from uart */
        data_size = this->separate_read_thread_started ? this->io->PrepareDataAfterInit() : this->io->PrepareDataBeforeInit();
        Log::Debug(std::to_string(data_size) + " bytes need to be parsed");
        if (0 == data_size && this->io->IsStopRequested()) {
            return;
        }

        if (this->message_classes_initialized) {
            this->ExecuteBPCallbacks(data_size);
        }

        while(message::Message::PARSE_RESULT_UNFINDHEADER == pres) {
            for(auto it = this->supported_msg_class.begin(); it != this->supported_msg_class.end(); ++it) {
                mres = it->second->IsMessageClassIdentifierMatch(this->io);
                if (MessageClass::MATCH_RESULT_NOT_MATCH == mres) {
                    Log::Trace("MessageClass not match.");
                    continue;
                }
                if (MessageClass::MATCH_RESULT_DATATRUNCATE == mres) {
                    Log::Trace("MessageClass data truncate.");
                    pres = message::Message::PARSE_RESULT_DATATRUNCATE;
                    break;
                }
                if (MessageClass::MATCH_RESULT_EXACT_MATCH == mres) {
                    Log::Trace("MessageClass match.");
                    pres = it->second->ProcessMessages(this->io);
                    current_cls_id = (MessageClass::MESSAGE_CLASS_E)it->second->GetMessageClassID();
                    break;
                }
            }

            if (message::Message::PARSE_RESULT_UNFINDHEADER == pres) {
                /* Drop one byte and parse again */
                if (this->message_classes_initialized) {
                    this->drop_byte_count++;
                }
                if (1 != this->io->Drop(1)) {
                    if (this->io->IsStopRequested()) {
                        break;
                    }
                    Log::Fatal("No byte can be drop.");
                    break;
                }
            } else {
                if (this->drop_byte_count > 0) {
                    Log::Warn("Received A Series Of Unknown Bytes, " + std::to_string(this->drop_byte_count) + " Bytes In Total, Drop Them. ");
                    this->drop_byte_count = 0;
                }
                if (message::Message::PARSE_RESULT_CONTENTERROR == pres) {
                    Log::Trace("Message content error.");
                    switch (current_cls_id) {
                        case MessageClass::MESSAGE_CLASS_GMF:
                            Log::Warn("Received One Package Of Error Gmf Message, Drop it.");
                            break;
                        case MessageClass::MESSAGE_CLASS_NMEA:
                            Log::Warn("Received One Package Of Error NMEA Message, Drop it.");
                            break;
                        default:
                            Log::Warn("Received One Package Of Error Unknown Message, Drop it.");
                            break;
                    }
                    if (1 != this->io->Drop(1)) {
                        if (this->io->IsStopRequested()) {
                            break;
                        }
                        Log::Fatal("No byte can be drop.");
                        break;
                    }
                    pres = message::Message::PARSE_RESULT_UNFINDHEADER;
                } else if (message::Message::PARSE_RESULT_DATATRUNCATE == pres) {
                    Log::Trace("Message data truncate.");
                    break;
                } else if (message::Message::PARSE_RESULT_UNPASSCHECK == pres) {
                    /* Drop one byte and parse again */
                    switch (current_cls_id) {
                        case MessageClass::MESSAGE_CLASS_GMF:
                            Log::Warn("Received One Package Of Error Gmf Message That Failed Validation, Drop it, Message ID: " + std::to_string(MessageClassGmf::msg_basic_info.MsgID) +
                                ", Message Name: " + std::string(MessageClassGmf::msg_basic_info.MsgName) + (MessageClassGmf::msg_basic_info.SlaveAnt ? "_1." : "."));
                            break;
                        case MessageClass::MESSAGE_CLASS_NMEA:
                            Log::Warn("Received One Package Of NMEA Message That Failed Validation, Drop it, Message Name: " + std::string(MessageClassNMEA::msg_basic_info.MsgName) +
                                (MessageClassNMEA::msg_basic_info.SlaveAnt ? "_1." : "."));
                            break;
                        case MessageClass::MESSAGE_CLASS_RESPONSE:
                            Log::Warn("Received One Package Of Response Message That Failed Validation, Drop it, Command Name: " + std::string(MessageClassResponse::msg_basic_info.CommandName));
                            break;
                        case MessageClass::MESSAGE_CLASS_OUTCONFIG:
                            Log::Warn("Received One Package Of OutConfig Entry That Failed Validation, Drop it, Config Name: " + std::string(MessageClassOutConfig::msg_basic_info.ConfigName));
                            break;
                        default:
                            Log::Warn("Received One Package Of Unknown Message, Drop it.");
                            break;
                    }
                    if (1 != this->io->Drop(1)) {
                        if (this->io->IsStopRequested()) {
                            break;
                        }
                        Log::Fatal("No byte can be drop.");
                        break;
                    }
                    pres = message::Message::PARSE_RESULT_UNFINDHEADER;
                } else { /**< Message::PARSE_RESULT_PARSEDONE */
                    /* if parse done, data in cycle buffer had been dropped. */
                    Log::Trace("Message parse done.");
                    pres = message::Message::PARSE_RESULT_UNFINDHEADER;
                }
            }
        }

        if (this->message_classes_initialized) {
            this->ExecuteAPCallbacks(data_size);
        }

        if (0 != this->delay_ms) {
            usleep(this->delay_ms * 1000);
        }
    }

    Result HuaceM7xxDriver::RegisterIOCallback(const std::string &cb_name, IOInterface::Callback cb, void *user) {
        return this->io->RegisterCallback(cb_name, cb, user);
    }

    Result HuaceM7xxDriver::UnregisterIOCallback(const std::string &cb_name) {
        return this->io->UnregisterCallback(cb_name);
    }

    bool HuaceM7xxDriver::IsIOCallbackRegistered(const std::string &cb_name) {
        return this->io->IsCallbackRegistered(cb_name);
    }

    Result HuaceM7xxDriver::RegisterMessageClass(std::shared_ptr<MessageClass> cls) {
        if (this->IsMessageClassRegistered(cls->GetMessageClassID())) {
            return Err;
        }
        this->supported_msg_class.insert({cls->GetMessageClassID(), cls});
        return Ok;
    }

    Result HuaceM7xxDriver::UnregisterMessageClass(uint32_t cls_id) {
        if (!this->IsMessageClassRegistered(cls_id)){
            return Err;
        }
        this->supported_msg_class.erase(cls_id);
        return Ok;
    }

    bool HuaceM7xxDriver::IsMessageClassRegistered(uint32_t cls_id) {
        return 0 != this->supported_msg_class.count(cls_id);
    }

    Result HuaceM7xxDriver::RegisterMessage(uint32_t cls_id, std::shared_ptr<message::Message> msg) {
        if (!this->IsMessageClassRegistered(cls_id)) {
            return Err;
        }
        if (this->IsMessageRegistered(cls_id, msg->GetMessageID())) {
            return Err;
        }
        return this->supported_msg_class.at(cls_id)->RegisterMessage(msg);
    }

    Result HuaceM7xxDriver::RegisterMessage(uint32_t cls_id, std::shared_ptr<message::Message> msg, const std::string &cb_name, message::Message::Callback cb, void *user) {
        if (!this->IsMessageClassRegistered(cls_id)) {
            return Err;
        }
        if (this->IsMessageRegistered(cls_id, msg->GetMessageID())) {
            return Err;
        }
        return this->supported_msg_class.at(cls_id)->RegisterMessage(msg, cb_name, cb, user);
    }

    Result HuaceM7xxDriver::UnregisterMessage(uint32_t cls_id, uint16_t msg_id) {
        if (!this->IsMessageRegistered(cls_id, msg_id)) {
            return Err;
        }
        return this->supported_msg_class.at(cls_id)->UnregisterMessage(msg_id);
    }

    bool HuaceM7xxDriver::IsMessageRegistered(uint32_t cls_id, uint16_t msg_id) {
        return this->IsMessageClassRegistered(cls_id)
            && this->supported_msg_class.at(cls_id)->supported_msgs.count(msg_id);
    }

    Result HuaceM7xxDriver::RegisterMessageCallback(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name, message::Message::Callback cb, void *user) {
        if (!this->IsMessageClassRegistered(cls_id)) {
            return Err;
        }
        if (!this->IsMessageRegistered(cls_id, msg_id)) {
            return Err;
        }
        return this->supported_msg_class.at(cls_id)->RegisterMessageCallback(msg_id, cb_name, cb, user);
    }

    Result HuaceM7xxDriver::UnregisterMessageCallback(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name) {
        if (!this->IsMessageRegistered(cls_id, msg_id)) {
            return Err;
        }
        return this->supported_msg_class.at(cls_id)->UnregisterMessageCallback(msg_id, cb_name);
    }

    bool HuaceM7xxDriver::IsMessageCallbackRegistered(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name) {
        return this->IsMessageRegistered(cls_id, msg_id)
            && this->supported_msg_class.at(cls_id)->supported_msgs.at(msg_id)->IsCallbackFnRegistered(cb_name);
    }

    Result HuaceM7xxDriver::RegisterMessageCallbackForEachMsg(uint32_t cls_id, const std::string &cb_name, message::Message::Callback cb, void *user) {
        Result res = Ok;
        for (auto it = this->supported_msg_class.at(cls_id)->supported_msgs.begin(); it != this->supported_msg_class.at(cls_id)->supported_msgs.end(); ++it) {
            if (Ok != this->RegisterMessageCallback(cls_id, it->first, cb_name, cb, user)) {
                Log::Warn("Failed to Register " + cb_name + " Callback Function for Class ID: " + std::to_string(cls_id) + " Message ID: " + std::to_string(it->first));
                res = Err;
            }
        }
        return res;
    }

    Result HuaceM7xxDriver::UnregisterMessageCallbackForEachMsg(uint32_t cls_id, const std::string &cb_name) {
        Result res = Ok;
        for (auto it = this->supported_msg_class.at(cls_id)->supported_msgs.begin(); it != this->supported_msg_class.at(cls_id)->supported_msgs.end(); ++it) {
            if (Ok != this->UnregisterMessageCallback(cls_id, it->first, cb_name)) {
                Log::Warn("Failed to Unregister " + cb_name + " Callback Function for Class ID: " + std::to_string(cls_id) + " Message ID: " + std::to_string(it->first));
                res = Err;
            }
        }
        return res;
    }

    void HuaceM7xxDriver::RegisterLogFnTrace(Log::LogFn fn) { Log::logger.Trace = fn; }
    void HuaceM7xxDriver::RegisterLogFnDebug(Log::LogFn fn) { Log::logger.Debug = fn; }
    void HuaceM7xxDriver::RegisterLogFnInfo(Log::LogFn fn) { Log::logger.Info = fn; }
    void HuaceM7xxDriver::RegisterLogFnWarn(Log::LogFn fn) { Log::logger.Warn = fn; }
    void HuaceM7xxDriver::RegisterLogFnError(Log::LogFn fn) { Log::logger.Error = fn; }
    void HuaceM7xxDriver::RegisterLogFnFatal(Log::LogFn fn) { Log::logger.Fatal = fn; }

    Result HuaceM7xxDriver::RegisterBPCallback(const std::string &name, HuaceM7xxDriver::Callback cb, void *user) {
        std::lock_guard<std::mutex> lock(this->bp_callbacks_mutex);
        if (0 != this->bp_callbacks.count(name)) {
            return Err;
        }
        this->bp_callbacks.insert({name, std::make_pair(cb, user)});
        return Ok;
    }

    Result HuaceM7xxDriver::UnregisterBPCallback(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->bp_callbacks_mutex);
        if (0 == this->bp_callbacks.count(name)) {
            return Err;
        }
        this->bp_callbacks.erase(name);
        return Ok;
    }

    bool HuaceM7xxDriver::IsBPCallbackRegistered(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->bp_callbacks_mutex);
        return 0 != this->bp_callbacks.count(name);
    }

    void HuaceM7xxDriver::ExecuteBPCallbacks(uint32_t data_size) {
        std::unordered_map<std::string, std::pair<Callback, void *>> callbacks_snapshot;
        {
            std::lock_guard<std::mutex> lock(this->bp_callbacks_mutex);
            callbacks_snapshot = this->bp_callbacks;
        }

        for (auto it = callbacks_snapshot.begin(); it != callbacks_snapshot.end(); ++it) {
            if (Ok != it->second.first(data_size, it->second.second)) {
                Log::Warn("Callback Function " + it->first + " Return Err.");
            }
        }
    }

    Result HuaceM7xxDriver::RegisterAPCallback(const std::string &name, HuaceM7xxDriver::Callback cb, void *user) {
        std::lock_guard<std::mutex> lock(this->ap_callbacks_mutex);
        if (0 != this->ap_callbacks.count(name)) {
            return Err;
        }
        this->ap_callbacks.insert({name, std::make_pair(cb, user)});
        return Ok;
    }

    Result HuaceM7xxDriver::UnregisterAPCallback(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->ap_callbacks_mutex);
        if (0 == this->ap_callbacks.count(name)) {
            return Err;
        }
        this->ap_callbacks.erase(name);
        return Ok;
    }

    bool HuaceM7xxDriver::IsAPCallbackRegistered(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->ap_callbacks_mutex);
        return 0 != this->ap_callbacks.count(name);
    }

    void HuaceM7xxDriver::ExecuteAPCallbacks(uint32_t data_size) {
        std::unordered_map<std::string, std::pair<Callback, void *>> callbacks_snapshot;
        {
            std::lock_guard<std::mutex> lock(this->ap_callbacks_mutex);
            callbacks_snapshot = this->ap_callbacks;
        }

        for (auto it = callbacks_snapshot.begin(); it != callbacks_snapshot.end(); ++it) {
            if (Ok != it->second.first(data_size, it->second.second)) {
                Log::Warn("Callback Function " + it->first + " Return Err.");
            }
        }
    }

} /* namespace huace_m7xx_ros_driver */
