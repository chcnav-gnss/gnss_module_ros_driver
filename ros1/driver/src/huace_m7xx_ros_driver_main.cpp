#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include <exception>

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "HuaceM7xxDriverNode");
    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~");

    try {
        huace_m7xx_ros_driver::HuaceM7xxDriverNode node(nh, private_nh);
        ros::spin();
    } catch (const std::exception &e) {
        ROS_FATAL("Failed to start HuaceM7xxDriverNode: %s", e.what());
        return 1;
    }

    return 0;
}
