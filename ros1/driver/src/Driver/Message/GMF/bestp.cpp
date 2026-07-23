#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
namespace gmf
{
    const std::string BestP::MESSAGE_NAME = "BESTPB";

    BestP::BestP() 
    {
        Log::Trace("BestP Ctor.");
    }

    BestP::~BestP()
    {
        Log::Trace("BestP Dtor.");
    }

    Result BestP::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName() + "_1", msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result BestP::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName() + "_1", msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result BestP::GetCancelSlaveAntMessageCmd(std::string &out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName() + "_1");
        return cmd.GetCmdString(out_cmd);
    }

    Result BestP::GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName() + "_1");
        return cmd.GetCmdString(out_cmd);
    }

} /* namespace gmf */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */
