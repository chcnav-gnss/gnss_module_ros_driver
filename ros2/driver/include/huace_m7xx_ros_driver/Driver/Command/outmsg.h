#ifndef __HUACE_M7XX_DRIVER_COMMAND_OUTMSG_H__
#define __HUACE_M7XX_DRIVER_COMMAND_OUTMSG_H__

#include "command.h"

namespace huace_m7xx_ros_driver
{
namespace command
{
class Outmsg : public Command
{
public:
    using Command::GetCmdString;
    enum OUTMSG_PORT_E {
        OUTMSG_PORT_SELF,
        OUTMSG_PORT_COM1,
        OUTMSG_PORT_COM2,
        OUTMSG_PORT_COM3,
    };
    enum OUTMSG_PERIOD_E {
        OUTMSG_PERIOD_ONSCHEDULE,
        OUTMSG_PERIOD_ONCHANGED,
        OUTMSG_PERIOD_ONNEW,
        OUTMSG_PERIOD_ONCE,
        OUTMSG_PERIOD_ONMARK,
        OUTMSG_PERIOD_20HZ,
        OUTMSG_PERIOD_10HZ,
        OUTMSG_PERIOD_5HZ,
        OUTMSG_PERIOD_2HZ,
        OUTMSG_PERIOD_1HZ,
        OUTMSG_PERIOD_2S,
        OUTMSG_PERIOD_5S,
        OUTMSG_PERIOD_10S,
        OUTMSG_PERIOD_15S,
        OUTMSG_PERIOD_20S,
        OUTMSG_PERIOD_30S,
        OUTMSG_PERIOD_60S,
        OUTMSG_PERIOD_120S,
        OUTMSG_PERIOD_MAX,
    };
    Outmsg(void) = delete;
    Outmsg(std::string msg_name, OUTMSG_PERIOD_E msg_period);
    Outmsg(OUTMSG_PORT_E port, std::string msg_name, OUTMSG_PERIOD_E msg_period);
    ~Outmsg();

    inline const std::string GetCommandName() override final {
        return this->COMMAND_NAME;
    }

    Result GetCmdString(std::string &out_cmd);

    static std::string GetOutmsgPortString(OUTMSG_PORT_E port);
    static OUTMSG_PORT_E GetOutmsgPortEnum(std::string str);
    static bool IsValidPort(OUTMSG_PORT_E port);
    static bool IsValidPort(std::string str);

    static bool IsValidMessageName(std::string msg_name);

    static std::string GetOutmsgPeriodString(OUTMSG_PERIOD_E period);
    static OUTMSG_PERIOD_E GetOutmsgPeriodEnum(std::string str);
    static bool IsValidPeriod(OUTMSG_PERIOD_E period);
    static bool IsValidPeriod(std::string str);

    static const std::string COMMAND_NAME;
    static const std::unordered_map<OUTMSG_PORT_E, std::string> port_map;
    static const std::unordered_map<OUTMSG_PERIOD_E, std::string> period_map;

    static const std::string UNSPECIFIED_MESSAGE_NAME;
    static const OUTMSG_PORT_E UNSPECIFIED_PORT = OUTMSG_PORT_SELF;
    static constexpr OUTMSG_PERIOD_E UNSPECIFIED_PERIOD = OUTMSG_PERIOD_MAX;

private:
    OUTMSG_PORT_E port;
    std::string msg_name;
    OUTMSG_PERIOD_E period;
}; /* class Outmsg */
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_COMMAND_OUTMSG_H__ */
