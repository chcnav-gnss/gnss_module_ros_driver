#!/bin/bash
set -e

if [ -z "${ROS_DISTRO:-}" ]; then
  if [ -f /opt/ros/noetic/setup.bash ]; then
    source /opt/ros/noetic/setup.bash
  elif [ -f /opt/ros/kinetic/setup.bash ]; then
    source /opt/ros/kinetic/setup.bash
  else
    echo "ROS 1 Kinetic or Noetic was not found under /opt/ros." >&2
    exit 1
  fi
elif [ "${ROS_DISTRO}" != "kinetic" ] && [ "${ROS_DISTRO}" != "noetic" ]; then
  echo "Unsupported ROS distribution: ${ROS_DISTRO}" >&2
  exit 1
fi

exec roslaunch huace_m7xx_ros_driver application.launch "$@"
