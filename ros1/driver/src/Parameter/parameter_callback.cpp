#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/Command/outmsg.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{

Result HuaceM7xxDriverNode::SetPeriodParameter(
    const std::string &message_name, const std::string &period,
    int32_t *error_code)
{
    if (error_code == nullptr || !m7xx_ || shutdown_started_.load()) {
        return Err;
    }

    *error_code = ROS_DRIVER_ERROR_CODE;
    Result result = Err;

    if (command::Outmsg::IsValidPeriod(period)) {
        result = GetOutmsgCommandResponseErrorCode(
            message_name, command::Outmsg::GetOutmsgPeriodEnum(period),
            error_code);
    } else if (period == "0") {
        result = GetOffmsgCommandResponseErrorCode(message_name, error_code);
    } else {
        ROS_WARN("Unsupported output period '%s' for %s.",
                 period.c_str(), message_name.c_str());
        *error_code = message::Response::INVALID_PARAMETER;
        return Err;
    }

    return result;
}

}  // namespace huace_m7xx_ros_driver
