#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

namespace huace_m7xx_ros_driver
{

void HuaceM7xxDriverNode::CreateServices()
{
    service_off_msg_ = nh_.advertiseService(
        "OFFMSG", &HuaceM7xxDriverNode::ServiceCallbackOffMsg, this);
    service_out_msg_ = nh_.advertiseService(
        "OUTMSG", &HuaceM7xxDriverNode::ServiceCallbackOutMsg, this);
}

}  // namespace huace_m7xx_ros_driver
