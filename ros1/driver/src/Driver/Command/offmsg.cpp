#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
namespace command
{
    const std::string Offmsg::COMMAND_NAME = "OFFMSG";

    Offmsg::Offmsg(void)
        : port(Outmsg::UNSPECIFIED_PORT), msg_name(Outmsg::UNSPECIFIED_MESSAGE_NAME)
    {
        Log::Trace("Offmsg Ctor.");
    }

    Offmsg::Offmsg(Outmsg::OUTMSG_PORT_E port)
        : port(port), msg_name(Outmsg::UNSPECIFIED_MESSAGE_NAME)
    {
        Log::Trace("Offmsg Ctor.");
    }

    Offmsg::Offmsg(std::string msg_name)
        : port(Outmsg::UNSPECIFIED_PORT), msg_name(msg_name)
    {
        Log::Trace("Offmsg Ctor.");
    }

    Offmsg::Offmsg(Outmsg::OUTMSG_PORT_E port, std::string msg_name)
        : port(port), msg_name(msg_name)
    {
        Log::Trace("Offmsg Ctor.");
    }

    Offmsg::~Offmsg()
    {
        Log::Trace("Offmsg Dtor.");
    }

    Result Offmsg::GetCmdString(std::string &out_cmd) {
        out_cmd.push_back(COMMAND_HEADER);
        out_cmd.append(COMMAND_NAME);

        if (Outmsg::UNSPECIFIED_PORT != this->port) {
            if (!Outmsg::IsValidPort(this->port)) {
                return Err;
            }
            out_cmd.push_back(PARAM_DELIMITER);
            out_cmd.append(Outmsg::GetOutmsgPortString(port));
        }

        if (Outmsg::UNSPECIFIED_MESSAGE_NAME != this->msg_name) {
            if (!Outmsg::IsValidMessageName(this->msg_name)) {
                return Err;
            }
            out_cmd.push_back(PARAM_DELIMITER);
            out_cmd.append(this->msg_name);
        }

        return Command::AppendCheckXOR(out_cmd);
    }

} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
