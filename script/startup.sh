#!/bin/bash
set -e

source /opt/ros/humble/setup.bash
exec ros2 launch huace_m7xx_ros_driver application.launch.py "$@"
