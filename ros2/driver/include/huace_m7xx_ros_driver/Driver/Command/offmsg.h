#ifndef __HUACE_M7XX_DRIVER_COMMAND_OFFMSG_H__
#define __HUACE_M7XX_DRIVER_COMMAND_OFFMSG_H__

#include "command.h"
#include "outmsg.h"

namespace huace_m7xx_ros_driver
{
namespace command
{
class Offmsg : public Command
{
public:
    using Command::GetCmdString;
    Offmsg(void);
    Offmsg(Outmsg::OUTMSG_PORT_E port);
    Offmsg(std::string msg_name);
    Offmsg(Outmsg::OUTMSG_PORT_E port, std::string msg_name);
    ~Offmsg();

    inline const std::string GetCommandName() override final {
        return this->COMMAND_NAME;
    }

    Result GetCmdString(std::string &out_cmd);

    static const std::string COMMAND_NAME;

private:
    Outmsg::OUTMSG_PORT_E port;
    std::string msg_name;
}; /* class Offmsg */
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_COMMAND_OFFMSG_H__ */
