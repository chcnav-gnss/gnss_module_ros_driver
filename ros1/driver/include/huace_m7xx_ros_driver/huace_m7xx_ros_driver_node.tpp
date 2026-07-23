#ifndef HUACE_M7XX_ROS_DRIVER_NODE_TPP
#define HUACE_M7XX_ROS_DRIVER_NODE_TPP

namespace huace_m7xx_ros_driver
{

template <typename MessageT>
bool HuaceM7xxDriverNode::SetPeriodParameter(
    const std::string &msg_name, const std::string &period,
    ros::Publisher &publisher, const std::string &topic_name,
    int32_t *error_code)
{
    if (error_code == nullptr) {
        return false;
    }

    const Result result = SetPeriodParameter(msg_name, period, error_code);
    if (result == Ok && *error_code == message::Response::OK && !publisher) {
        publisher = nh_.advertise<MessageT>(topic_name, 10);
    }
    return result == Ok && *error_code == message::Response::OK;
}

} /* namespace huace_m7xx_ros_driver */

#endif /* HUACE_M7XX_ROS_DRIVER_NODE_TPP */
