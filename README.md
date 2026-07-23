# Huace M7xx ROS Driver

This repository provides ROS 1 and ROS 2 drivers for Huace M7xx GNSS
receivers. ROS 1 and ROS 2 are kept in separate directories so that each
version can be built and maintained independently.

## Repository Layout

```text
.
├── ros1/
│   ├── driver/       # ROS 1 driver package: huace_m7xx_ros_driver
│   └── rtcm_msgs/    # ROS 1 RTCM message package: rtcm_msgs
└── ros2/
    └── driver/       # ROS 2 driver package: huace_m7xx_ros_driver
```

The directory name `driver` is only the source directory name. The ROS package
name remains `huace_m7xx_ros_driver`.

More ROS 1 or ROS 2 dependency packages may be added in the future. They
should be placed under the matching version directory:

```text
ros1/
├── driver/
├── rtcm_msgs/
└── <future_ros1_package>/

ros2/
├── driver/
└── <future_ros2_package>/
```

Each added package must keep its own ROS package metadata and build files.
ROS 1 packages use `catkin`; ROS 2 packages use `ament` and are built with
`colcon`.

## Version Documentation

- [ROS 1 documentation](ros1/driver/README.md)
- [ROS 1 Chinese documentation](ros1/driver/README_CN.md)
- [ROS 2 documentation](ros2/driver/README.md)

The version-specific documentation contains the supported environment,
dependency installation, build commands, runtime parameters, topics,
services, message definitions, and NTRIP integration instructions.

## ROS 1

The ROS 1 driver supports:

- Ubuntu 16.04 with ROS Kinetic
- Ubuntu 20.04 with ROS Noetic
- ARM64 builds
- `NMEA/GGA` and `BESTP` publishing
- RTCM input through `rtcm_msgs/Message`
- `OUTMSG` and `OFFMSG` services

The ROS 1 source package `rtcm_msgs` is included at
`ros1/rtcm_msgs`. Place `ros1/driver` and `ros1/rtcm_msgs` as sibling packages
in a catkin workspace `src/` directory before building.

## ROS 2

The ROS 2 driver targets:

- Ubuntu 22.04
- ROS 2 Humble
- ARM64 builds
- `NMEA/GGA` and `BESTP` publishing
- RTCM input through `rtcm_msgs::msg::Message`

ROS 2 uses the ROS 2 `rtcm_msgs` dependency. Do not use the ROS 1
`ros1/rtcm_msgs` package directly in a ROS 2 `colcon` workspace.

## Build Isolation

Use separate workspaces for ROS 1 and ROS 2. Do not share generated build
directories between distributions or build systems.

```text
ros1_ws/
├── src/
├── build/
├── devel/
└── install/

ros2_ws/
├── src/
├── build/
├── install/
└── log/
```

For complete commands and runtime examples, read the documentation for the
specific ROS version.

## NTRIP

The repository provides the driver and RTCM message definitions, but does not
include an NTRIP client. Use a compatible external NTRIP client and follow the
NTRIP section in the corresponding ROS 1 or ROS 2 documentation.

## License

See the license file in the corresponding driver directory.
