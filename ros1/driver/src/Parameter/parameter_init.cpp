#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

namespace huace_m7xx_ros_driver
{

void HuaceM7xxDriverNode::LoadParameters()
{
    private_nh_.param<std::string>("COM1", com1_, "/dev/ttyUSB0");
    private_nh_.param("baudrate", baudrate_, 1500000);
    private_nh_.param<std::string>("frame_id", frame_id_, "gnss");
    private_nh_.param<std::string>("nmea_gga_period", nmea_gga_period_, "5");
    private_nh_.param<std::string>("gmf_bestp_period", gmf_bestp_period_, "0.1");
}

}  // namespace huace_m7xx_ros_driver
