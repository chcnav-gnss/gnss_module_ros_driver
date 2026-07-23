#ifndef __HUACE_M7XX_DRIVER_MESSAGE_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_H__

#include "../IOInterface/serialport.h"
#include "../huace_m7xx_common.h"
#include "../Command/outmsg.h"
#include "../Command/offmsg.h"
#include <cstdint>
#include <memory>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace huace_m7xx_ros_driver
{
namespace message
{
class Message
{
public:
    enum PARSE_RESULT_E {
        PARSE_RESULT_PARSEDONE    = 1,
        PARSE_RESULT_UNFINDHEADER = 2,
        PARSE_RESULT_DATATRUNCATE = 3,
        PARSE_RESULT_UNPASSCHECK  = 4,
        PARSE_RESULT_CONTENTERROR = 5,
    };

    using Callback = std::function<Result(const std::vector<uint8_t> &msg, void *user)>;

    Message(void);
    virtual ~Message();

    virtual inline uint16_t GetMessageID() = 0;
    virtual inline const std::string GetMessageName() = 0;

    virtual Result GetRequestMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd);
    virtual Result GetRequestMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd);
    virtual Result GetCancelMessageCmd(std::string &out_cmd);
    virtual Result GetCancelMessageCmd(std::shared_ptr<std::string> out_cmd);

    virtual Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd);
    virtual Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd);
    virtual Result GetCancelSlaveAntMessageCmd(std::string &out_cmd);
    virtual Result GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd);

    virtual PARSE_RESULT_E ParseBinary(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseASCII(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseAbbreviatedASCII(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseNMEA(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseRTCM(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseOutConfig(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseResponse(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParseCoreDump(std::shared_ptr<IOInterface> io);
    virtual PARSE_RESULT_E ParsePort(std::shared_ptr<IOInterface> io);

    Result RegisterCallbackFn(const std::string &name, Callback cb, void *user);
    Result UnregisterCallbackFn(const std::string &name);
    bool IsCallbackFnRegistered(const std::string &name);
    void ExecuteCallbacks(const std::vector<uint8_t> &message);

    static bool IsValidMessageGroup(const std::string group);

    static constexpr uint32_t CRC32_LENGTH = 4;

    std::unordered_map<std::string, std::pair<Callback, void *>> callbacks;
    std::mutex callbacks_mutex;

    static const std::unordered_set<std::string> supported_message_group_name_set;

}; /* class Message */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_H__ */
