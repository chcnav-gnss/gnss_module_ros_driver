#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"
#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"

namespace huace_m7xx_ros_driver
{

const std::string HuaceM7xxDriverNode::DEFAULT_MESSAGE_PERIOD = "0";

rcl_interfaces::msg::SetParametersResult HuaceM7xxDriverNode::parameter_callback(const std::vector<rclcpp::Parameter> &parameters)
{
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;

    for (const auto &parameter : parameters) {
        const std::string &param_name = parameter.get_name();
        const std::string param_value = parameter.as_string();

        if ("nmea_gga_period" == param_name) {
            if (Ok == this->SetPeriodParameter<nmea_msgs::msg::Sentence>(this->m7xx, message::GGA::MESSAGE_NAME, param_value, result, this->publisher_nmea_gga_, "NMEA/GGA")) {
                this->nmea_gga_period_ = param_value;
                RCLCPP_INFO(this->get_logger(), "Set NMEA GGA Period: %s", this->nmea_gga_period_.c_str());
            }
            continue;
        }

        if ("gmf_bestp_period" == param_name) {
            if (Ok == this->SetPeriodParameter<huace_m7xx_ros_driver::msg::BESTP>(this->m7xx, message::gmf::BestP::MESSAGE_NAME, param_value, result, this->publisher_gmf_bestp_, "BESTP")) {
                this->gmf_bestp_period_ = param_value;
                RCLCPP_INFO(this->get_logger(), "Set BESTP Period: %s", this->gmf_bestp_period_.c_str());
            }
            continue;
        }

        result.successful = false;
        result.reason = "Unsupported period parameter: " + param_name;
        return result;
    }

    return result;
}

Result HuaceM7xxDriverNode::GetOutmsgCommandResponseErrorCode(
    std::shared_ptr<HuaceM7xxDriver> driver,
    std::string msg_name,
    command::Outmsg::OUTMSG_PERIOD_E period,
    int32_t *error_code)
{
    command::Outmsg cmd(msg_name, period);
    return this->GetCommandResponseErrorCode(driver, cmd, error_code);
}

Result HuaceM7xxDriverNode::GetOffmsgCommandResponseErrorCode(
    std::shared_ptr<HuaceM7xxDriver> driver,
    std::string msg_name,
    int32_t *error_code)
{
    command::Offmsg cmd(msg_name);
    return this->GetCommandResponseErrorCode(driver, cmd, error_code);
}

Result HuaceM7xxDriverNode::SetPeriodParameter(
    std::shared_ptr<HuaceM7xxDriver> driver,
    std::string msg_name,
    std::string param,
    rcl_interfaces::msg::SetParametersResult &result)
{
    int32_t error_code = 0;
    if (command::Outmsg::IsValidPeriod(param)) {
        if (Ok != this->GetOutmsgCommandResponseErrorCode(
                driver, msg_name, command::Outmsg::GetOutmsgPeriodEnum(param), &error_code)) {
            RCLCPP_WARN(this->get_logger(), "GetOutmsgCommandResponseErrorCode Failed.");
        }
    } else if (DEFAULT_MESSAGE_PERIOD == param) {
        if (Ok != this->GetOffmsgCommandResponseErrorCode(driver, msg_name, &error_code)) {
            RCLCPP_WARN(this->get_logger(), "GetOffmsgCommandResponseErrorCode Failed.");
        }
    } else {
        result.successful = false;
        result.reason = "unknown period.";
        return Err;
    }

    result.successful = error_code == message::Response::OK;
    return result.successful ? Ok : Err;
}

} /* namespace huace_m7xx_ros_driver */
