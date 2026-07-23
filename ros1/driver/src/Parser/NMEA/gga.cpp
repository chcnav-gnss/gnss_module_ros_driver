#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

namespace huace_m7xx_ros_driver
{

void HuaceM7xxDriverNode::ProcessStdMsgHeader(std_msgs::Header *header)
{
    header->stamp = ros::Time::now();
    header->frame_id = frame_id_;
}

Result HuaceM7xxDriverNode::ProcessMessageNMEAGGA(
    const std::vector<uint8_t> &message_data, void *user)
{
    (void)user;
    nmea_msgs::Sentence output;
    ProcessStdMsgHeader(&output.header);
    output.sentence.assign(message_data.begin(), message_data.end());

    if (publisher_nmea_gga_) {
        publisher_nmea_gga_.publish(output);
    }
    return Ok;
}

}  // namespace huace_m7xx_ros_driver
