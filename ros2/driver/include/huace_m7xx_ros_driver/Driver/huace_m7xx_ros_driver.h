#ifndef __HUACE_M7XX_DRIVER_H__
#define __HUACE_M7XX_DRIVER_H__

#include "huace_m7xx_common.h"
#include "huace_m7xx_log.h"
#include "IOInterface/iointerface.h"
#include "IOInterface/serialport.h"
#include "IOInterface/fileport.h"
#include "MessageClass/message_class.h"
#include "Message/message.h"
#include "Command/command.h"
#include <map>
#include <mutex>

namespace huace_m7xx_ros_driver
{

class HuaceM7xxDriver
{
public:
    using Callback = std::function<Result(uint32_t data_size, void *user)>;
    HuaceM7xxDriver() = delete;
    HuaceM7xxDriver(HuaceM7xxDriver &) = delete;
    HuaceM7xxDriver(HuaceM7xxDriver &&) = delete;
    HuaceM7xxDriver(std::shared_ptr<SerialPort> port, uint32_t baudrate);
    HuaceM7xxDriver(std::shared_ptr<FilePort> port, uint32_t delay_ms);

    ~HuaceM7xxDriver();

    void Init(void);
    Result AutoSetBaudrate(uint32_t baudrate);
    Result SendMessage(const uint8_t *msg, uint32_t length);
    Result SendMessage(const std::vector<uint8_t> &msg);
    Result SendMessage(const std::shared_ptr<std::vector<uint8_t>> msg);
    Result SendMessage(const std::string &msg);
    Result SendMessage(const std::shared_ptr<std::string> msg);
    Result RequestMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, command::Outmsg::OUTMSG_PERIOD_E period);
    Result RequestSlaveAntMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, command::Outmsg::OUTMSG_PERIOD_E period);
    Result CancelMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id);
    Result CancelSlaveAntMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id);
    Result ExecuteCommand(command::Command &cmd);
    Result ExecuteCommand(const std::shared_ptr<command::Command> cmd);
    Result ExecuteCommand(const std::string &cmd_str);
    Result ExecuteCommand(const std::shared_ptr<std::string> cmd_str);
    void RequestStop(void);
    void ProcessMessages(void);
    void StartSeparateReadThread(void);

    Result RegisterIOCallback(const std::string &cb_name, IOInterface::Callback cb, void *user);
    Result UnregisterIOCallback(const std::string &cb_name);
    bool IsIOCallbackRegistered(const std::string &cb_name);

    Result RegisterMessageClass(std::shared_ptr<MessageClass> cls);
    Result UnregisterMessageClass(uint32_t cls_id);
    bool IsMessageClassRegistered(uint32_t cls_id);

    Result RegisterMessage(uint32_t cls_id, std::shared_ptr<message::Message> msg);
    Result RegisterMessage(uint32_t cls_id, std::shared_ptr<message::Message> msg, const std::string &cb_name, message::Message::Callback cb, void *user);
    Result UnregisterMessage(uint32_t cls_id, uint16_t msg_id);
    bool IsMessageRegistered(uint32_t cls_id, uint16_t msg_id);

    Result RegisterMessageCallback(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name, message::Message::Callback cb, void *user);
    Result UnregisterMessageCallback(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name);
    bool IsMessageCallbackRegistered(uint32_t cls_id, uint16_t msg_id, const std::string &cb_name);

    Result RegisterMessageCallbackForEachMsg(uint32_t cls_id, const std::string &cb_name, message::Message::Callback cb, void *user);
    Result UnregisterMessageCallbackForEachMsg(uint32_t cls_id, const std::string &cb_name);

    static void RegisterLogFnTrace(Log::LogFn fn);
    static void RegisterLogFnDebug(Log::LogFn fn);
    static void RegisterLogFnInfo(Log::LogFn fn);
    static void RegisterLogFnWarn(Log::LogFn fn);
    static void RegisterLogFnError(Log::LogFn fn);
    static void RegisterLogFnFatal(Log::LogFn fn);

    Result RegisterBPCallback(const std::string &name, HuaceM7xxDriver::Callback cb, void *user);
    Result UnregisterBPCallback(const std::string &name);
    bool IsBPCallbackRegistered(const std::string &name);
    void ExecuteBPCallbacks(uint32_t data_size);

    Result RegisterAPCallback(const std::string &name, HuaceM7xxDriver::Callback cb, void *user);
    Result UnregisterAPCallback(const std::string &name);
    bool IsAPCallbackRegistered(const std::string &name);
    void ExecuteAPCallbacks(uint32_t data_size);
private:
    std::map<uint32_t, std::shared_ptr<MessageClass>> supported_msg_class;
    std::shared_ptr<IOInterface> io;
    uint32_t delay_ms;
    bool message_classes_initialized;
    bool separate_read_thread_started;
    uint32_t drop_byte_count;
    std::mutex write_mutex;
    std::mutex bp_callbacks_mutex;
    std::mutex ap_callbacks_mutex;
    std::unordered_map<std::string, std::pair<Callback, void *>> bp_callbacks;
    std::unordered_map<std::string, std::pair<Callback, void *>> ap_callbacks;
}; /* class HuaceM7xxDriver */

} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_H__ */
