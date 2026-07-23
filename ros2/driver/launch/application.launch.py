from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("COM1", default_value="/dev/ttyUSB0"),
        DeclareLaunchArgument("baudrate", default_value="1500000"),
        DeclareLaunchArgument("frame_id", default_value="gnss"),
        DeclareLaunchArgument("nmea_gga_period", default_value="\"5\""),
        DeclareLaunchArgument("gmf_bestp_period", default_value="\"0.1\""),
        Node(
            package="huace_m7xx_ros_driver",
            executable="huace_m7xx_ros_driver_app",
            name="HuaceM7xxDriverNode",
            namespace="/",
            output="both",
            parameters=[{
                "COM1": LaunchConfiguration("COM1"),
                "baudrate": LaunchConfiguration("baudrate"),
                "frame_id": LaunchConfiguration("frame_id"),
                "nmea_gga_period": LaunchConfiguration("nmea_gga_period"),
                "gmf_bestp_period": LaunchConfiguration("gmf_bestp_period"),
            }],
            arguments=["--ros-args", "--log-level", "info"],
        ),
    ])
