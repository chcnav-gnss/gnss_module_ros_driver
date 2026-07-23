# Huace M7xx ROS Driver

- Connect to the M7xx GNSS module through the `COM1` serial port
- Publish `NMEA/GGA`
- Publish `BESTP`
- Subscribe to `/RTCM` and forward it to the M7xx
- Provide the basic `OUTMSG` and `OFFMSG` services

## Environment Checks

Before starting, make sure that:

- The M7xx is connected to the Linux host
- The serial device node exists, for example `/dev/ttyUSB0`
- The current user has permission to access the serial device

## Dependencies

- Ubuntu 22.04
- ROS2 Humble
- `nmea_msgs`
- `rtcm_msgs`

Install the dependencies:

```bash
sudo apt update
sudo apt install -y ros-humble-nmea-msgs ros-humble-rtcm-msgs
```

## Build

Run the following commands from the workspace root:

```bash
cd <workspace>
source /opt/ros/humble/setup.bash
colcon build --packages-select huace_m7xx_ros_driver
source install/setup.bash
```

## Run

### 1. Run Directly

If you only want to verify serial communication first and do not need NTRIP, start the driver node directly:

```bash
ros2 run huace_m7xx_ros_driver huace_m7xx_ros_driver_app --ros-args \
  -p COM1:=/dev/ttyUSB0 \
  -p baudrate:=1500000 \
  -p frame_id:=gnss \
  -p nmea_gga_period:=\"5\" \
  -p gmf_bestp_period:=\"0.1\"
```

`nmea_gga_period` and `gmf_bestp_period` are declared as string parameters in the driver.
In the current `application.launch.py`, these two parameters are declared with quoted string default values:

```python
DeclareLaunchArgument("nmea_gga_period", default_value="\"5\"")
DeclareLaunchArgument("gmf_bestp_period", default_value="\"0.1\"")
```

### 2. Use Launch

```bash
ros2 launch huace_m7xx_ros_driver application.launch.py \
  COM1:=/dev/ttyUSB0 \
  baudrate:=1500000 \
  frame_id:=gnss \
  nmea_gga_period:=\"5\" \
  gmf_bestp_period:=\"0.1\"
```

If you only need the basic driver functionality, start it as shown above.
If you need NTRIP, continue reading the NTRIP section below.

### 3. Component Loading

The driver supports loading through `rclcpp_components`.

The node type that can be loaded is:

- `huace_m7xx_ros_driver::HuaceM7xxDriverNode`

If your system uses `component_container`, you can load this node directly as a composable node.

## Interfaces

### Published Topics

- `/NMEA/GGA`
- `/BESTP`
- `/RTCM` (subscribed)

### Services

- `/OFFMSG`
- `/OUTMSG`

### Main Parameters

- `COM1`: Serial device, default `/dev/ttyUSB0`
- `baudrate`: Baud rate, default `1500000`
- `frame_id`: `frame_id` in the ROS Header, default `gnss`
- `nmea_gga_period`: GGA output period; `0` disables output
- `gmf_bestp_period`: BESTP output period; `0` disables output

## NTRIP

This project provides NTRIP support through `ntrip_client`.
The node to start is:

- ROS package: `ntrip_client`
- Executable: `ntrip_ros.py`

### Installation

Install the external NTRIP client:

```bash
sudo apt update
sudo apt install -y ros-humble-ntrip-client
```

After installation, run the following command to confirm that the package is available:

```bash
ros2 pkg prefix ntrip_client
```

### Data Flow

The external `ntrip_client` requires the following remappings:

- `/nmea` -> `/NMEA/GGA`
- `/rtcm` -> `/RTCM`

After startup, the data flow is:

1. The M7xx outputs GGA
2. The driver publishes `/NMEA/GGA`
3. `ntrip_ros.py` subscribes to `/NMEA/GGA` and connects to the NTRIP caster
4. The caster returns RTCM
5. `ntrip_ros.py` publishes `/RTCM`
6. The driver subscribes to `/RTCM` and forwards the RTCM data to the M7xx

### Using `launch.py`

If you need to enable NTRIP, there are two ways to do so:

1. Start the driver first, then start `ntrip_client` separately
2. Add the `ntrip_client` node directly to your `launch.py`

To start the driver and `ntrip_client` in the same `launch.py`, add them as follows.

First, add the NTRIP-related `DeclareLaunchArgument` entries:

```python
DeclareLaunchArgument("host", default_value=""),
DeclareLaunchArgument("port", default_value=""),
DeclareLaunchArgument("mountpoint", default_value=""),
DeclareLaunchArgument("username", default_value=""),
DeclareLaunchArgument("password", default_value="\"YOUR_PASSWORD\""),
```

Then add the following node after the driver node in `LaunchDescription([...])`:

```python
Node(
    package="ntrip_client",
    executable="ntrip_ros.py",
    name="ntrip_ros",
    namespace="/",
    output="both",
    parameters=[{
        "host": LaunchConfiguration("host"),
        "port": LaunchConfiguration("port"),
        "mountpoint": LaunchConfiguration("mountpoint"),
        "ntrip_version": "Ntrip/2.0",
        "authenticate": True,
        "username": LaunchConfiguration("username"),
        "password": LaunchConfiguration("password"),
        "ssl": False,
        "cert": "None",
        "key": "None",
        "ca_cert": "None",
        "rtcm_frame_id": "odom",
        "nmea_max_length": 128,
        "nmea_min_length": 3,
        "rtcm_message_package": "rtcm_msgs",
        "reconnect_attempt_max": 10,
        "reconnect_attempt_wait_seconds": 5,
        "rtcm_timeout_seconds": 4,
    }],
    remappings=[
        ("/nmea", "/NMEA/GGA"),
        ("/rtcm", "/RTCM"),
    ],
    arguments=["--ros-args", "--log-level", "info"],
),
```

After adding it, one `launch.py` can start both the driver and `ntrip_client`.

If you do not want to modify `launch.py`, start `ntrip_client` separately after starting the current driver:

```bash
ros2 run ntrip_client ntrip_ros.py --ros-args \
  -p host:=YOUR_NTRIP_HOST \
  -p port:=YOUR_PORT \
  -p mountpoint:=YOUR_MOUNTPOINT \
  -p username:=YOUR_USERNAME \
  -p password:=\"YOUR_PASSWORD\" \
  -p ntrip_version:=Ntrip/2.0 \
  -p authenticate:=true \
  -p ssl:=false \
  -p rtcm_frame_id:=odom \
  -p rtcm_message_package:=rtcm_msgs \
  -p nmea_max_length:=128 \
  -r /nmea:=/NMEA/GGA \
  -r /rtcm:=/RTCM
```

Common `ntrip_client` parameters:

- `host`: NTRIP caster address
- `port`: NTRIP caster port
- `mountpoint`: Mountpoint
- `username`: Username
- `password`: Password
- `ntrip_version`: `Ntrip/2.0`
- `authenticate`: `True`
- `ssl`: `False`
- `rtcm_frame_id`: `odom`
- `rtcm_message_package`: `rtcm_msgs`
- `nmea_max_length`: `128`

Fill in these parameters according to your NTRIP service information.

### Parameter Notes

Pay special attention to the fact that the following parameters are strings in the actual code:

- `nmea_gga_period`
- `gmf_bestp_period`
- `password`

The recommended form is:

```python
DeclareLaunchArgument("nmea_gga_period", default_value="\"5\"")
DeclareLaunchArgument("gmf_bestp_period", default_value="\"0.1\"")
DeclareLaunchArgument("password", default_value="\"YOUR_PASSWORD\"")
```

This prevents ROS 2 from automatically inferring types when processing `launch` parameters, reducing parameter type mismatch issues.
This note mainly applies to the `launch.py` scenario; this handling is generally unnecessary when passing parameters directly on the command line.

### Verification Steps (If You Open a New Terminal, Ensure the Same Environment Is Sourced)

1. Confirm that the driver is publishing `/NMEA/GGA`

```bash
ros2 topic echo /NMEA/GGA
```

2. Confirm that the nodes have started

```bash
ros2 node list
```

Under normal circumstances, you should see at least:

- `/HuaceM7xxDriverNode`
- `/ntrip_ros`

3. Confirm that the RTCM topic is available

```bash
ros2 topic list | grep RTCM
ros2 topic echo /RTCM
```

If `/RTCM` does not appear, check the following in order:

- `host`
- `port`
- `mountpoint`
- `username`
- `password`
- Whether the network can access the NTRIP caster

## How to Add Messages

It is recommended to add messages incrementally: first add low-level recognition, then add ROS publishing, and finally decide whether a custom `.msg` is needed.

The conclusion is:

- If the new message is an NMEA text sentence, you can usually reuse `nmea_msgs::msg::Sentence` directly, without adding a new ROS `.msg`
- If the new message is a GMF binary message and subscribers need structured fields, add a custom ROS message under `msg/`

Regardless of the message type, follow the steps below.

### 1. Determine the Message Type

This is the first and most important step.

#### 1.1 NMEA Text Messages

Characteristics:

- The serial port receives a complete text sentence
- It usually starts with `$` and ends with `\r\n`
- Typical command names are short text identifiers such as `GGA`, `GST`, `VTG`, and `ZDA`

For this type of message, reuse `nmea_msgs::msg::Sentence` directly.

Advantages:

- No new ROS `.msg` needs to be maintained
- Minimal changes
- Convenient for users to verify

#### 1.2 GMF Binary Messages

Characteristics:

- The low-level message is a binary message with a fixed structure
- Subscribers usually care about its fields rather than the raw byte stream
- It is similar to `BESTP` in the current project

This type of message generally requires:

- Adding a ROS message definition under `msg/`
- Registering it in `rosidl_generate_interfaces(...)` in `CMakeLists.txt`
- Mapping low-level structure fields to ROS message fields in the parser layer

If you are unsure which type applies, use this simple rule:

- If you only want to convert the raw text into a topic, use the NMEA approach
- If you need to split out fields for subscribers, use the GMF approach

### 2. Add the Low-Level Message Definition

The purpose of the low-level message definition is to make the driver recognize the message.

#### 2.1 For an NMEA Message

You usually need to modify:

- `include/huace_m7xx_ros_driver/Driver/Message/nmea.h`
- `src/Driver/Message/nmea.cpp`
- `include/huace_m7xx_ros_driver/Driver/Message/NMEA/`
- `src/Driver/Message/NMEA/`

The required changes usually include:

1. Add an enum value to `NMEA_MESSAGE_E`
2. Add the message name to `supported_message_name_set`
3. Add a message class header and implementation, for example:
   - `include/huace_m7xx_ros_driver/Driver/Message/NMEA/xxx.h`
   - `src/Driver/Message/NMEA/xxx.cpp`

This message class must contain at least:

- `MESSAGE_ID`
- `MESSAGE_NAME`
- Inheritance from `NMEA`

#### 2.2 For a GMF Message

You usually need to modify:

- `include/huace_m7xx_ros_driver/Driver/Message/gmf.h`
- `src/Driver/Message/gmf.cpp`
- `include/huace_m7xx_ros_driver/Driver/Message/GMF/`
- `src/Driver/Message/GMF/`

The required changes usually include:

1. Add the message name to `supported_message_name_set`
2. Define the low-level message structure
3. Add `MESSAGE_ID`
4. Add `MESSAGE_NAME`
5. Add field descriptions or status-bit conversion logic if needed

Do not skip this step.
If you only add `GMF/xxx.h` and `GMF/xxx.cpp` without updating `supported_message_name_set` in `gmf.cpp`, the message-name validation later in the driver may still fail to recognize the message.

### 3. Register the Low-Level Message Class

Adding a `Message` class alone is not enough; the driver must also know to process it.

#### 3.1 NMEA Message Registration

- `src/Driver/MessageClass/message_nmea.cpp`

Two changes are required:

1. `#include` the new message header
2. Insert it into `supported_msgs` in the `MessageClassNMEA` constructor

Example:

```cpp
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/xxx.h"

this->supported_msgs.insert({
  message::NMEA::NMEA_MESSAGE_XXX,
  std::make_shared<message::XXX>()
});
```

#### 3.2 GMF Message Registration

- `src/Driver/MessageClass/message_gmf.cpp`

The process is the same as for NMEA:

- Include the header
- Register it in `supported_msgs`

If this step is omitted, the symptoms are usually:

- You have already sent `OUTMSG` to the device
- The serial port may have actually received data
- But the driver does not recognize the message as a supported message

### 4. Add ROS Parsing and Publishing Logic

This layer converts low-level messages into ROS topics.

#### 4.1 Add a Parser File

Place the file according to the message type:

- NMEA: `src/Parser/NMEA/`
- GMF: `src/Parser/GMF/`

For example:

- `src/Parser/NMEA/xxx.cpp`
- `src/Parser/GMF/xxx.cpp`

This layer usually needs to:

1. Receive the raw low-level message
2. Fill in `std_msgs/Header`
3. Assemble the final ROS message
4. Publish it through the corresponding publisher

#### 4.2 Declare the Interface in the Node Header

Modify:

- `include/huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h`

You usually need to add:

- A declaration for `ProcessMessage...(...)` or `ProcessGmfMessage...(...)`
- The corresponding publisher member
- The corresponding period parameter member

For an NMEA text message, the publisher is usually:

```cpp
rclcpp::Publisher<nmea_msgs::msg::Sentence>::SharedPtr publisher_nmea_xxx_;
```

For a structured GMF message, the publisher is usually the custom ROS message type you added.

### 5. Connect the Message to Node Initialization

Modify:

- `src/huace_m7xx_ros_driver_node.cpp`

This layer usually requires three additions.

#### 5.1 Create the Publisher

Add it to:

- `CreateNMEAPublisher()`
- or `CreateGmfPublisher()`

Example:

```cpp
this->CreatePublisher<MessageT>(publisher_xxx_, xxx_period_, "TOPIC_NAME");
```

#### 5.2 Request the Message from the Device at Startup

Add it to:

- `RequestNMEA()`
- or `RequestGmf()`

Example:

```cpp
this->RequestMessage(this->m7xx, msg_class, msg_id, msg_name, period);
```

#### 5.3 Register the Callback

Add it to:

- `RegisterNMEACallback()`
- or `RegisterGmfCallback()`

Example:

```cpp
this->RegisterCallback(
  this->m7xx,
  msg_class,
  msg_id,
  msg_name,
  "1",
  std::bind(&HuaceM7xxDriverNode::ProcessMessageXXX, this, std::placeholders::_1, std::placeholders::_2),
  nullptr);
```

All three steps are required:

- Without a publisher, the topic cannot be published
- Without a request, the device will not start outputting the message
- Without a callback, the driver will not know how to process a received message

### 6. Add the Parameter Control Path

If you want users to control the message output period through `launch.py` or `ros2 param set`, just as they currently control `GGA` and `BESTP`, complete the period parameter path.

#### 6.1 Declare the Parameter

Modify:

- `src/Parameter/parameter_init.cpp`

Add a period parameter, for example:

```cpp
this->DeclarePeriodParameter(this->xxx_period_, "xxx_period", "0", "Output period of XXX.");
```

It is recommended to use `0` as the default value, meaning that the code supports the message but output is disabled by default.

#### 6.2 Apply Parameter Changes Dynamically

Modify:

- `src/Parameter/parameter_callback.cpp`

The required changes are:

1. Identify the parameter name (including the header if needed)
2. Call `SetPeriodParameter(...)`
3. Update the local cached variable
4. Print a log message

Example:

```cpp
if ("xxx_period" == param_name) {
  if (Ok == this->SetPeriodParameter<MessageT>(...)) {
    this->xxx_period_ = param_value;
  }
  continue;
}
```

This allows the same logic to be reused whether the user:

- Passes the parameter from `launch` at startup
- Changes the parameter dynamically at runtime

### 7. Add the Parameter to `launch`

Modify:

- `launch/application.launch.py`

Add a default configuration entry to `parameters=[{...}]`, for example:

```python
"xxx_period": "0",
```

Benefits:

- Users can see the available message options by opening the launch file
- To verify a message, they only need to change `0` to a valid period
- They do not need to search for the parameter name themselves

It is recommended that all newly added messages follow the same rule:

- Supported but disabled by default: `"0"`
- Clearly state in the documentation that `0` means disabled

### 8. Add New Source Files to the Build System

Modify:

- `CMakeLists.txt`

#### 8.1 Low-Level Driver Source Files

If you add a low-level message implementation, add it to `DRIVER_SOURCES`, for example:

```cmake
src/Driver/Message/NMEA/xxx.cpp
```

or:

```cmake
src/Driver/Message/GMF/xxx.cpp
```

#### 8.2 Parser Source Files

If you add a parser or publishing file, also add it to the node target, for example:

```cmake
src/Parser/NMEA/xxx.cpp
```

or:

```cmake
src/Parser/GMF/xxx.cpp
```

If this layer is omitted, the most common symptoms are:

- The build succeeds partly but linking fails
- The corresponding symbol cannot be found at runtime

### 9. Add `msg/` Only When Structured ROS Messages Are Needed

If the new message is not simple text but requires structured fields like `BESTP`, modify:

- `msg/`
- `rosidl_generate_interfaces(...)` in `CMakeLists.txt`

The recommended process is:

1. Write the `.msg` file first
2. Register it in `rosidl_generate_interfaces(...)`
3. Add any dependent submessages and dependencies
4. Map the low-level structure fields to ROS message fields in the parser layer

If you are only adding an NMEA text message, you can skip this step completely.

### 10. Build and Verify

Build:

```bash
cd <workspace>
source /opt/ros/humble/setup.bash
colcon build --packages-select huace_m7xx_ros_driver
source install/setup.bash
```

The recommended verification order is as follows. If you open a new terminal, make sure the same environment is sourced:

1. Change the new message's `xxx_period` from `0` to a valid value in `launch/application.launch.py`
2. Start the driver
3. Check whether the topic appears:

```bash
ros2 topic list
```

4. Check whether the message is actually being published:

```bash
ros2 topic echo /<your_topic_name>
```

### 11. Recommended Order for Adding Messages

Expand the driver in this order:

1. Add an NMEA text message
   This requires the fewest changes and is most suitable for open-source users to extend themselves.

2. Add a structured GMF message
   This involves `.msg` files, field mapping, and dependency chains, so it is more complex.

3. Add and verify one message at a time
   Avoid adding multiple messages at once; otherwise, it becomes difficult to identify which layer is incomplete when a problem occurs.

### 12. Minimal Checklist

After adding a message, check at least the following before submitting:

- The low-level `Message` class has been added
- It has been registered with `MessageClass`
- The publisher, parameter, and callback have been declared in the node header
- The publisher has been created in `src/huace_m7xx_ros_driver_node.cpp`
- The device has been requested to output the message at startup
- The message callback has been registered
- The parameter has been declared in `parameter_init.cpp`
- The parameter has been handled in `parameter_callback.cpp`
- The default parameter has been exposed in `launch/application.launch.py`
- The new `.cpp` file has been added to `CMakeLists.txt`
- If a structured ROS message is needed, `msg/` and `rosidl_generate_interfaces(...)` have been updated together

As long as this entire chain is completed, users can follow the implementation of the existing messages in the project and continue adding new messages reliably.
