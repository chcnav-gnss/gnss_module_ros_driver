#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include "huace_m7xx_ros_driver/Driver/Message/nmea.h"
#include "huace_m7xx_ros_driver/Driver/Message/gmf.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace huace_m7xx_ros_driver
{
namespace {

std::string NormalizeBestpMessageName(const std::string &message)
{
    if (message.size() >= 2 && message.compare(message.size() - 2, 2, "_1") == 0) {
        std::string converted = message;
        converted.insert(converted.end() - 2, 'B');
        return converted;
    }
    return message + "B";
}

}  // namespace

const std::string HuaceM7xxDriverNode::DEFAULT_PARAM_STRING = "NOTSET";

void HuaceM7xxDriverNode::ServiceCallbackOffMsg(const std::shared_ptr<huace_m7xx_ros_driver::srv::OffMsg::Request> request,
                                                std::shared_ptr<huace_m7xx_ros_driver::srv::OffMsg::Response> response)
{
    RCLCPP_INFO(this->get_logger(), "Executing OFFMSG Command.");
    rcl_interfaces::msg::SetParametersResult result;
    std::string msg_name = request->message;
    std::transform(msg_name.begin(), msg_name.end(), msg_name.begin(), ::tolower);

    const std::string bestp_msg_name = NormalizeBestpMessageName(request->message);

    if (message::NMEA::IsValidMessageName(msg_name)) {
        result = this->set_parameter(rclcpp::Parameter("nmea_" + msg_name + "_period", DEFAULT_MESSAGE_PERIOD));
    } else if (message::gmf::Gmf::IsValidMessageName(bestp_msg_name)) {
        result = this->set_parameter(rclcpp::Parameter("gmf_" + msg_name + "_period", DEFAULT_MESSAGE_PERIOD));
    } else if (DEFAULT_PARAM_STRING == request->message) {
        command::Offmsg cmd;
        (void)this->GetCommandResponseErrorCode(this->m7xx, cmd, &response->error_code);
        return;
    } else {
        result.successful = false;
    }

    response->error_code = result.successful ? message::Response::OK : message::Response::INVALID_PARAMETER;
}

void HuaceM7xxDriverNode::ServiceCallbackOutMsg(const std::shared_ptr<huace_m7xx_ros_driver::srv::OutMsg::Request> request,
                                                std::shared_ptr<huace_m7xx_ros_driver::srv::OutMsg::Response> response)
{
    RCLCPP_INFO(this->get_logger(), "Executing OUTMSG Command.");
    rcl_interfaces::msg::SetParametersResult result;
    std::string msg_name = request->message;
    std::transform(msg_name.begin(), msg_name.end(), msg_name.begin(), ::tolower);

    const std::string bestp_msg_name = NormalizeBestpMessageName(request->message);

    if (message::NMEA::IsValidMessageName(msg_name)) {
        result = this->set_parameter(rclcpp::Parameter("nmea_" + msg_name + "_period", request->period));
    } else if (message::gmf::Gmf::IsValidMessageName(bestp_msg_name)) {
        result = this->set_parameter(rclcpp::Parameter("gmf_" + msg_name + "_period", request->period));
    } else {
        result.successful = false;
    }

    response->error_code = result.successful ? message::Response::OK : message::Response::INVALID_PARAMETER;
}


} /* namespace huace_m7xx_ros_driver */
