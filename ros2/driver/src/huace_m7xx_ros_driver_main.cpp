#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<huace_m7xx_ros_driver::HuaceM7xxDriverNode>());
    rclcpp::shutdown();
    return 0;
}
