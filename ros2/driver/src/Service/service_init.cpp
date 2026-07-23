#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

namespace huace_m7xx_ros_driver
{

void HuaceM7xxDriverNode::CreateServices(void)
{
    this->service_off_msg_ = this->create_service<huace_m7xx_ros_driver::srv::OffMsg>(
        "OFFMSG",
        std::bind(&HuaceM7xxDriverNode::ServiceCallbackOffMsg, this, std::placeholders::_1, std::placeholders::_2),
        rclcpp::ServicesQoS().get_rmw_qos_profile());

    this->service_out_msg_ = this->create_service<huace_m7xx_ros_driver::srv::OutMsg>(
        "OUTMSG",
        std::bind(&HuaceM7xxDriverNode::ServiceCallbackOutMsg, this, std::placeholders::_1, std::placeholders::_2),
        rclcpp::ServicesQoS().get_rmw_qos_profile());

}

} /* namespace huace_m7xx_ros_driver */
