#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"

namespace huace_m7xx_ros_driver
{
    Result HuaceM7xxDriverNode::ProcessMessageNMEAGGA(const std::vector<uint8_t> &msg, void *user)
    {
        RCLCPP_DEBUG(this->get_logger(), "Publishing NMEA GGA Message.");
        (void) user;
        auto pub_msg = std::make_unique<nmea_msgs::msg::Sentence>();

        this->ProcessStdMsgHeader(&pub_msg->header);
        pub_msg->sentence.assign(msg.begin(), msg.end());

        if (this->publisher_nmea_gga_) {
            this->publisher_nmea_gga_->publish(std::move(pub_msg));
        }
        return Ok;
    }
} /* namespace huace_m7xx_ros_driver */
