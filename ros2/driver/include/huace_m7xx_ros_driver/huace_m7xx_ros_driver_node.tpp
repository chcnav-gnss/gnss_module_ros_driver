#include "rclcpp/rclcpp.hpp"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_ros_driver.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{
    template <typename MessageT>
    void HuaceM7xxDriverNode::CreatePublisher(typename rclcpp::Publisher<MessageT>::SharedPtr &publisher, std::string period, const std::string &topic_name, const rclcpp::QoS &qos)
    {
        if (command::Outmsg::IsValidPeriod(period))
        {
            publisher = this->create_publisher<MessageT>(topic_name, qos);
        }
    }

    template <typename MessageT>
    Result HuaceM7xxDriverNode::SetPeriodParameter(std::shared_ptr<HuaceM7xxDriver> driver, std::string msg_name, std::string param, rcl_interfaces::msg::SetParametersResult &result, 
                                                   typename rclcpp::Publisher<MessageT>::SharedPtr &publisher, const std::string &topic_name, const rclcpp::QoS &qos)
    {
        int32_t error_code = 0;
        if (command::Outmsg::IsValidPeriod(param))
        {
            if (Ok == this->GetOutmsgCommandResponseErrorCode(driver, msg_name, command::Outmsg::GetOutmsgPeriodEnum(param), &error_code))
            {
                if (nullptr == publisher)
                {
                    publisher = this->create_publisher<MessageT>(topic_name, qos);
                }
            }
            else
            {
                RCLCPP_WARN(this->get_logger(), "GetOutmsgCommandResponseErrorCode Failed.");
            }
        }
        else if (DEFAULT_MESSAGE_PERIOD == param)
        {
            if (Ok != this->GetOffmsgCommandResponseErrorCode(driver, msg_name, &error_code))
            {
                RCLCPP_WARN(this->get_logger(), "GetOffmsgCommandResponseErrorCode Failed.");
            }
        }
        else
        {
            result.successful = false;
            result.reason = "unknown period.";
            return Err;
        }
        result.successful = error_code == message::Response::OK;
        return result.successful ? Ok : Err;
    }
} /* namespace huace_m7xx_ros_driver */
