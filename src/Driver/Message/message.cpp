#include "huace_m7xx_ros_driver/Driver/Message/message.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    const std::unordered_set<std::string> Message::supported_message_group_name_set = {
        "DGRB"
    };

    Message::Message(void) { }

    Message::~Message() { }

    Result Message::GetRequestMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName(), msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result Message::GetRequestMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName(), msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result Message::GetCancelMessageCmd(std::string &out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName());
        return cmd.GetCmdString(out_cmd);
    }

    Result Message::GetCancelMessageCmd(std::shared_ptr<std::string> out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName());
        return cmd.GetCmdString(out_cmd);
    }

    Result Message::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) {
        (void) msg_period;
        (void) out_cmd;
        Log::Error("\"GetRequestSlaveAntMessageCmd\" function not implemented");
        return Err;
    }

    Result Message::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) {
        (void) msg_period;
        (void) out_cmd;
        Log::Error("\"GetRequestSlaveAntMessageCmd\" function not implemented");
        return Err;
    }

    Result Message::GetCancelSlaveAntMessageCmd(std::string &out_cmd) {
        (void) out_cmd;
        Log::Error("\"GetCancelSlaveAntMessageCmd\" function not implemented");
        return Err;
    }

    Result Message::GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd) {
        (void) out_cmd;
        Log::Error("\"GetCancelSlaveAntMessageCmd\" function not implemented");
        return Err;
    }

    Message::PARSE_RESULT_E Message::ParseBinary(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseBinary\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseASCII(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseASCII\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseAbbreviatedASCII(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseAbbreviatedASCII\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseNMEA(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseNMEA\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseRTCM(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseRTCM\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseOutConfig(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseOutConfig\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseResponse(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseResponse\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParseCoreDump(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParseCoredump\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Message::PARSE_RESULT_E Message::ParsePort(std::shared_ptr<IOInterface> io) {
        (void) io;
        Log::Error("\"ParsePort\" function not implemented");
        return PARSE_RESULT_UNFINDHEADER;
    }

    Result Message::RegisterCallbackFn(const std::string &name, Callback cb, void *user) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        if (0 != this->callbacks.count(name)) {
            return Err;
        }
        this->callbacks.insert({name, std::pair(cb, user)});
        Log::Debug("Callback \"" + name + "\" Registetered.");
        return Ok;
    }

    Result Message::UnregisterCallbackFn(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        if (0 == this->callbacks.count(name)) {
            return Err;
        }
        this->callbacks.erase(name);
        Log::Debug("Callback \"" + name + "\" Unegistetered.");
        return Ok;
    }

    bool Message::IsCallbackFnRegistered(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        return 0 != this->callbacks.count(name);
    }

    void Message::ExecuteCallbacks(const std::vector<uint8_t> &message) {
        std::unordered_map<std::string, std::pair<Callback, void *>> callbacks_snapshot;
        {
            std::lock_guard<std::mutex> lock(this->callbacks_mutex);
            callbacks_snapshot = this->callbacks;
        }

        for (auto it = callbacks_snapshot.begin(); it != callbacks_snapshot.end(); ++it) {
            if (Ok != it->second.first(message, it->second.second)) {
                Log::Warn("Callback Function " + it->first + " Return Err.");
            }
        }
    }

    bool Message::IsValidMessageGroup(const std::string group) {
        return 0 != Message::supported_message_group_name_set.count(group);
    }
} /* namespace messsage */
} /* namespace huace_m7xx_ros_driver */
