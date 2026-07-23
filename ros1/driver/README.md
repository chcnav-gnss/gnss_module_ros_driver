# Huace M7xx ROS Driver

- Connects to the M7xx device through the `COM1` serial port
- Publishes `NMEA/GGA`
- Publishes `BESTP`
- Subscribes to `/RTCM` and forwards RTCM data to the M7xx
- Provides the `OUTMSG` and `OFFMSG` services

## Environment Check

Before starting, confirm that:

- The M7xx is connected to the Linux host
- The serial device exists, for example `/dev/ttyUSB0`
- The current user or Docker container has permission to access the serial device
- The corresponding ROS 1 distribution is installed and can connect to `roscore`

This version has been built and runtime-tested in the following ARM64
environments:

| Operating system | ROS version | Compiler |
| --- | --- | --- |
| Ubuntu 16.04 Xenial | ROS Kinetic | GCC 5.4 |
| Ubuntu 20.04 Focal | ROS Noetic | GCC 9.4 |

Ubuntu 16.04 and Ubuntu 20.04 should use different catkin workspaces. The
source code can be the same, but the `build/`, `devel/`, and `install/`
directories must not be shared.

## Dependencies

- Ubuntu 16.04 + ROS 1 Kinetic, or Ubuntu 20.04 + ROS 1 Noetic
- `roscpp`
- `std_msgs`
- `message_generation`
- `message_runtime`
- `nmea_msgs`
- `rtcm_msgs`

`rtcm_msgs` is included in this repository as a ROS 1 source package:

```text
ros1/rtcm_msgs
```

It defines `rtcm_msgs/Message`, which contains a Header and a sequence of raw
RTCM bytes. Do not place two packages with the same name in one workspace.

### Ubuntu 20.04 / ROS Noetic

```bash
sudo apt update
sudo apt install -y build-essential ros-noetic-nmea-msgs
```

### Ubuntu 16.04 / ROS Kinetic

```bash
sudo apt update
sudo apt install -y build-essential ros-kinetic-nmea-msgs
```

The Kinetic package sources do not provide a usable `ros-kinetic-rtcm-msgs`
package, so use the `ros1/rtcm_msgs` source package included in this
repository.

## Build

Place `ros1/driver` and `ros1/rtcm_msgs` in the `src/`
directory of a catkin workspace, then run the commands below from the
workspace root.

### Ubuntu 20.04 / ROS Noetic

```bash
cd <catkin_workspace>
source /opt/ros/noetic/setup.bash
catkin_make
source devel/setup.bash
```

### Ubuntu 16.04 / ROS Kinetic

```bash
cd <catkin_workspace>
source /opt/ros/kinetic/setup.bash
catkin_make
source devel/setup.bash
```

If the workspace was previously built with another ROS distribution, clean the
build products first:

```bash
rm -rf build devel install
```

The ROS 1 node-layer directory structure corresponds to the ROS 2 version, but
uses the ROS 1 `roscpp`, services, publishers, and message types:

```text
include/huace_m7xx_ros_driver/
  huace_m7xx_ros_driver_node.h
  huace_m7xx_ros_driver_node.tpp

src/
  huace_m7xx_ros_driver_node.cpp
  Parameter/
    parameter_init.cpp
    parameter_callback.cpp
  Parser/
    NMEA/gga.cpp
    GMF/bestp.cpp
    response.cpp
  Service/
    service_init.cpp
    service_callback.cpp
```

`src/Driver/` contains the low-level serial, protocol parsing, and command
implementation. Splitting the ROS 1 node layer does not change the low-level
driver interface.

## Run

### 1. Run Directly

To verify serial communication without NTRIP, start the driver node directly:

```bash
rosrun huace_m7xx_ros_driver huace_m7xx_ros_driver_node \
  _COM1:=/dev/ttyUSB0 \
  _baudrate:=1500000 \
  _frame_id:=gnss \
  _nmea_gga_period:=5 \
  _gmf_bestp_period:=0.1
```

`nmea_gga_period` and `gmf_bestp_period` are string parameters. The supported
periods are defined by the device `OUTMSG` command, for example `0.1`, `1`,
and `5`. A value of `0` means that the driver does not request this message at
startup.

### 2. Use a Launch File

```bash
roslaunch huace_m7xx_ros_driver application.launch \
  COM1:=/dev/ttyUSB0 \
  baudrate:=1500000 \
  frame_id:=gnss \
  nmea_gga_period:=5 \
  gmf_bestp_period:=0.1
```

### 3. Run in Docker: Ubuntu 20.04 / ROS Noetic

The Docker container must use the host network and explicitly pass through the
M7xx serial device:

```bash
docker run --name ros1-noetic -it \
  --network host \
  --device=/dev/ttyUSB0:/dev/ttyUSB0 \
  -v "$HOME/m7xx_ros1_ws:/root/catkin_ws" \
  -w /root/catkin_ws \
  ros:noetic-ros-base-focal bash
```

Build and run inside the container:

```bash
source /opt/ros/noetic/setup.bash
catkin_make
source devel/setup.bash
roslaunch huace_m7xx_ros_driver application.launch COM1:=/dev/ttyUSB0
```

### 4. Run in Docker: Ubuntu 16.04 / ROS Kinetic

```bash
docker run --name ros1-kinetic-xenial -it \
  --network host \
  --device=/dev/ttyUSB0:/dev/ttyUSB0 \
  -v "$HOME/m7xx_ros1_kinetic_ws:/root/catkin_ws" \
  -w /root/catkin_ws \
  ros:kinetic-ros-base-xenial bash
```

Run inside the container:

```bash
source /opt/ros/kinetic/setup.bash
catkin_make
source devel/setup.bash
roslaunch huace_m7xx_ros_driver application.launch COM1:=/dev/ttyUSB0
```

## Interfaces

### Published Topics

- `/NMEA/GGA`, type `nmea_msgs/Sentence`
- `/BESTP`, type `huace_m7xx_ros_driver/BESTP`

### Subscribed Topics

- `/RTCM`, type `rtcm_msgs/Message`

### Services

- `/OFFMSG`, type `huace_m7xx_ros_driver/OffMsg`
- `/OUTMSG`, type `huace_m7xx_ros_driver/OutMsg`

### Main Parameters

- `~COM1`: serial device, default `/dev/ttyUSB0`
- `~baudrate`: baud rate, default `1500000`
- `~frame_id`: ROS Header `frame_id`, default `gnss`
- `~nmea_gga_period`: GGA output period, default `5`
- `~gmf_bestp_period`: BESTP output period, default `0.1`

Use the service to change the output period after startup:

```bash
rosservice call /OUTMSG "message: 'GGA'
period: '1'"

rosservice call /OFFMSG "message: 'GGA'"
```

ROS 1 `rosparam set` only changes the parameter server. It does not trigger
device reconfiguration. Changes to `COM1`, `baudrate`, or `frame_id` require
restarting the node.

## NTRIP

This project does not include an NTRIP client, but it can be used with an
external ROS 1 `ntrip_client` package. The NTRIP client must not open the M7xx
serial port; the serial port must remain exclusively owned by this driver.

### Installation

Install the ROS 1 version of `ntrip_client` in the same catkin workspace:

```bash
cd <catkin_workspace>/src
git clone --branch ros https://github.com/LORD-MicroStrain/ntrip_client.git

cd <catkin_workspace>
# Ubuntu 20.04 / ROS Noetic
source /opt/ros/noetic/setup.bash
# For Ubuntu 16.04 / ROS Kinetic, use:
# source /opt/ros/kinetic/setup.bash
rosdep install --from-paths src --ignore-src -r -y
catkin_make
source devel/setup.bash
```

The `ntrip_client` commands below have been verified on Ubuntu 20.04 / ROS
Noetic. On Ubuntu 16.04 / ROS Kinetic, confirm that the selected client is
compatible with both ROS Kinetic and Python 2. This repository provides the
driver and `rtcm_msgs`, but does not include an NTRIP client.

Confirm that the package is available:

```bash
rospack find ntrip_client
```

### Data Flow

The NTRIP client should use these ROS interfaces:

- Subscribe to `/NMEA/GGA`, type `nmea_msgs/Sentence`
- Publish `/RTCM`, type `rtcm_msgs/Message`

The data flow is:

1. The M7xx outputs GGA
2. The driver publishes `/NMEA/GGA`
3. The NTRIP client receives GGA and connects to the caster
4. The caster returns RTCM
5. The NTRIP client publishes `/RTCM`
6. The driver subscribes to `/RTCM` and forwards the raw RTCM bytes to the M7xx

### Start `ntrip_client` Separately (Noetic)

Start the M7xx driver first. In another terminal, load the same ROS environment
and start the NTRIP client:

```bash
source /opt/ros/noetic/setup.bash
source <catkin_workspace>/devel/setup.bash

rosrun ntrip_client ntrip_ros.py \
  __name:=ntrip_client \
  _host:=YOUR_NTRIP_HOST \
  _port:=YOUR_NTRIP_PORT \
  _mountpoint:=YOUR_MOUNTPOINT \
  _authenticate:=true \
  _username:=YOUR_USERNAME \
  _password:=YOUR_PASSWORD \
  _ssl:=false \
  _rtcm_message_package:=rtcm_msgs \
  _nmea_max_length:=128 \
  /nmea:=/NMEA/GGA \
  /rtcm:=/RTCM
```

This ROS 1 node uses the absolute topics `/nmea` and `/rtcm`. The remaps above
connect them to the driver's `/NMEA/GGA` and `/RTCM`.

The GGA output from the M7xx contains `\r\n`, so its actual length can exceed
the 82-byte NMEA limit. Set `_nmea_max_length:=128`; otherwise
`ntrip_client` may reject the GGA and the caster will not return RTCM.

The `ntrip_version` request header can be set according to the caster:

- Leave it empty for casters that do not require an `Ntrip-Version` header
- Set it to `Ntrip/2.0` for casters that explicitly require NTRIP v2

### Integrate into the Same Launch File (Noetic)

After installing the external `ntrip_client`, it can also be added to
`huace_m7xx_ros_driver/launch/application.launch`. Do not commit account
credentials to the repository. Pass them through launch arguments or a local
launch file excluded from version control.

Add these launch arguments:

```xml
<arg name="ntrip_host" default="" />
<arg name="ntrip_port" default="2101" />
<arg name="ntrip_mountpoint" default="" />
<arg name="ntrip_username" default="" />
<arg name="ntrip_password" default="" />
```

Then add the following after the M7xx driver node:

```xml
<node pkg="ntrip_client"
      type="ntrip_ros.py"
      name="ntrip_client"
      output="screen">
  <param name="host" value="$(arg ntrip_host)" />
  <param name="port" value="$(arg ntrip_port)" />
  <param name="mountpoint" value="$(arg ntrip_mountpoint)" />
  <param name="authenticate" value="true" />
  <param name="username" value="$(arg ntrip_username)" />
  <param name="password" value="$(arg ntrip_password)" />
  <param name="ssl" value="false" />
  <param name="rtcm_message_package" value="rtcm_msgs" />
  <param name="nmea_max_length" value="128" />
  <remap from="/nmea" to="/NMEA/GGA" />
  <remap from="/rtcm" to="/RTCM" />
</node>
```

Start both nodes with one launch command:

```bash
roslaunch huace_m7xx_ros_driver application.launch \
  COM1:=/dev/ttyUSB0 \
  ntrip_host:=YOUR_NTRIP_HOST \
  ntrip_port:=YOUR_NTRIP_PORT \
  ntrip_mountpoint:=YOUR_MOUNTPOINT \
  ntrip_username:=YOUR_USERNAME \
  ntrip_password:=YOUR_PASSWORD
```

### Common Parameters

- `host`: NTRIP caster address
- `port`: caster port, default `2101`
- `mountpoint`: mountpoint
- `authenticate`: whether username and password authentication is used
- `username`: NTRIP username
- `password`: NTRIP password
- `ntrip_version`: NTRIP protocol version request header; empty or `Ntrip/2.0`
- `ssl`: whether to use TLS
- `cert`, `key`, `ca_cert`: certificate configuration for TLS
- `rtcm_message_package`: set to `rtcm_msgs` to match the driver's
  `rtcm_msgs/Message` subscription
- `nmea_max_length`: set to `128` for M7xx GGA output to avoid rejection caused
  by the trailing `\r\n`

```bash
rostopic echo -n 1 /NMEA/GGA
rostopic hz /RTCM
rostopic echo -n 1 /BESTP
```

For NMEA GGA, a quality field of `4` usually indicates an RTK fixed solution.
`BESTP.pos_type.type` equal to `4` means `NARROW_INT`, which is a multi-frequency
RTK fixed solution.

If `/RTCM` has no output, check the following in order:

- Caster address, port, and mountpoint
- NTRIP username and password
- Whether the NTRIP client subscribes to `/NMEA/GGA`
- Whether the NTRIP client publishes `/RTCM` as `rtcm_msgs/Message`
- Network connectivity from the X5 to the caster

## Adding a Message

Add messages incrementally: first add low-level recognition, then add ROS
publishing, and finally decide whether a new custom `.msg` is required.

The main rules are:

- NMEA text messages normally reuse `nmea_msgs::Sentence`; no new ROS `.msg`
  is required
- A GMF binary message needs a new message definition only when subscribers
  need structured fields

### 1. Identify the Message Category

#### 1.1 NMEA Text Messages

Typical characteristics:

- A complete text sentence is received from the serial port
- It usually starts with `$` and ends with `\r\n`
- Typical message names are `GGA`, `GST`, `VTG`, and `ZDA`

Reuse `nmea_msgs::Sentence` for these messages. This keeps the change small and
makes validation with `rostopic echo` convenient.

#### 1.2 GMF Binary Messages

Typical characteristics:

- The low-level payload is a fixed binary structure
- Subscribers need structured fields
- The message is similar to the current `BESTP` message

These messages normally require:

- A new ROS 1 message definition under `msg/`
- Registration in `add_message_files(...)` in `CMakeLists.txt`
- Dependencies declared in `generate_messages(...)`
- Mapping of low-level structure fields to the ROS message in the node callback

### 2. Add the Low-Level Message Definition

The low-level message definition allows the driver to recognize a new M7xx
message.

#### 2.1 NMEA Messages

Usually modify:

- `include/huace_m7xx_ros_driver/Driver/Message/nmea.h`
- `src/Driver/Message/nmea.cpp`
- `include/huace_m7xx_ros_driver/Driver/Message/NMEA/`
- `src/Driver/Message/NMEA/`

Add:

1. An enum value in `NMEA_MESSAGE_E`
2. The message name to `supported_message_name_set`
3. A new message class header and implementation
4. `MESSAGE_ID` and `MESSAGE_NAME`, with inheritance from `NMEA`

#### 2.2 GMF Messages

Usually modify:

- `include/huace_m7xx_ros_driver/Driver/Message/gmf.h`
- `src/Driver/Message/gmf.cpp`
- `include/huace_m7xx_ros_driver/Driver/Message/GMF/`
- `src/Driver/Message/GMF/`

Add:

1. The message name to `supported_message_name_set`
2. The low-level payload structure
3. `MESSAGE_ID` and `MESSAGE_NAME`
4. Any required field or status-bit conversion logic

Adding only `GMF/xxx.h` and `GMF/xxx.cpp` is insufficient. If the message is
not registered in the supported-message list, later message-name validation
will still reject it.

### 3. Register the Message Class

After adding a message class, register it with the corresponding
`MessageClass`.

#### 3.1 NMEA Registration

Modify `src/Driver/MessageClass/message_nmea.cpp`:

```cpp
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/xxx.h"

supported_msgs.insert({
    message::NMEA::NMEA_MESSAGE_XXX,
    std::make_shared<message::XXX>()
});
```

#### 3.2 GMF Registration

Modify `src/Driver/MessageClass/message_gmf.cpp` in the same way: include the
new header and insert the message into `supported_msgs`.

If this step is missed, the device may output data after `OUTMSG`, but the
driver will not recognize it as a supported message.

### 4. Add ROS Parsing and Publishing

ROS 1 keeps the corresponding `src/Parser/` directories from the ROS 2
structure, but replaces ROS 2 APIs with `roscpp` APIs. Put message callbacks,
ROS message conversion, and publishing logic in:

- NMEA: `src/Parser/NMEA/`
- GMF: `src/Parser/GMF/`
- RESPONSE: `src/Parser/response.cpp`
- Node interface declarations:
  `include/huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h`

Normally:

1. Declare `ProcessMessage...(...)` or `ProcessGmfMessage...(...)` in the node
   header
2. Add the corresponding `ros::Publisher` and period members
3. Fill `std_msgs::Header` in the parser callback
4. Populate and publish the final ROS 1 message

NMEA text messages can use:

```cpp
nmea_msgs::Sentence output;
output.sentence.assign(message_data.begin(), message_data.end());
publisher_nmea_xxx_.publish(output);
```

GMF messages should define a structured ROS message and map each field from the
low-level payload structure.

### 5. Connect the Message to Node Initialization

Adding a message normally requires:

1. Creating a publisher
2. Requesting the device to output the message at startup
3. Registering the low-level message callback

The corresponding ROS 1 files are:

- Node initialization and generic publisher/request/callback flow:
  `src/huace_m7xx_ros_driver_node.cpp`
- NMEA ROS conversion and `std_msgs/Header`:
  `src/Parser/NMEA/gga.cpp`
- GMF ROS conversion:
  `src/Parser/GMF/bestp.cpp`
- RESPONSE command waiting and error codes:
  `src/Parser/response.cpp`
- Parameter loading:
  `src/Parameter/parameter_init.cpp`
- `/OUTMSG` and `/OFFMSG` period handling:
  `src/Parameter/parameter_callback.cpp` and
  `include/huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.tpp`
- Service creation:
  `src/Service/service_init.cpp`
- Service request handling:
  `src/Service/service_callback.cpp`

Connect the message to the initialization flow in
`src/huace_m7xx_ros_driver_node.cpp`, for example:

```cpp
CreateNMEAPublisher();
RegisterNMEACallback();
RequestNMEA();
```

For a generic NMEA message, add the corresponding publisher, period, and
callback declarations to the node header and extend the existing
`CreateNMEAPublisher()`, `RegisterNMEACallback()`, and `RequestNMEA()` flow.

Put NMEA conversion in the corresponding file under `src/Parser/NMEA/`. A
typical publishing block is:

```cpp
nmea_msgs::Sentence output;
ProcessStdMsgHeader(&output.header);
output.sentence.assign(message_data.begin(), message_data.end());
publisher_nmea_gga_.publish(output);
```

Without a publisher, no topic is published. Without `RequestMessage(...)`, the
device does not start outputting the message. Without a callback, the driver
does not process received data.

### 6. Add Output Period Control

The startup period for a new message should be added as a private node
parameter:

- Add `xxx_period_` to the node header
- Read it with `private_nh_.param(...)` in `LoadParameters()`
- Add the corresponding `<arg>` and `<param>` to
  `launch/application.launch`

Runtime changes to the message output period should extend the `OUTMSG` and
`OFFMSG` service handling.

The recommended default is `0`, meaning that the driver supports the message
but does not request it at startup. Set a valid period through a launch
parameter or the `/OUTMSG` service when testing.

### 7. Add the Parameter to the Launch File

For example, modify `launch/application.launch`:

```xml
<arg name="nmea_xxx_period" default="0" />

<node pkg="huace_m7xx_ros_driver"
      type="huace_m7xx_ros_driver_node"
      name="HuaceM7xxDriverNode"
      output="screen">
  <param name="nmea_xxx_period" value="$(arg nmea_xxx_period)" />
</node>
```

The parameter can then be overridden at startup:

```bash
roslaunch huace_m7xx_ros_driver application.launch nmea_xxx_period:=1
```

### 8. Add New Source Files to the Build

Modify `CMakeLists.txt`.

#### 8.1 Low-Level Driver Sources

Add new low-level message implementation files to `DRIVER_SOURCES`, for
example:

```cmake
src/Driver/Message/NMEA/xxx.cpp
```

or:

```cmake
src/Driver/Message/GMF/xxx.cpp
```

#### 8.2 Node Sources

The ROS 1 node layer is split into `Parameter/`, `Parser/`, and `Service/`.
New node-layer `.cpp` files must be added to the
`huace_m7xx_ros_driver_node` target in `CMakeLists.txt`.

If a source file is omitted, common symptoms are link errors or missing
runtime functionality.

### 9. Add `msg/` Only When a Structured ROS Message Is Needed

NMEA text messages normally do not require a new `.msg`. For a structured GMF
message such as `BESTP`:

1. Add a `.msg` file under `msg/`
2. Register it in `add_message_files(...)` in `CMakeLists.txt`
3. Declare dependencies in `generate_messages(DEPENDENCIES ...)`
4. Ensure `catkin_package(CATKIN_DEPENDS ... message_runtime)` contains the
   runtime dependency
5. Populate and publish the message in the node callback

### 10. Build and Verify

Build on Ubuntu 20.04 / ROS Noetic:

```bash
cd <catkin_workspace>
source /opt/ros/noetic/setup.bash
catkin_make --pkg huace_m7xx_ros_driver
source devel/setup.bash
```

Build on Ubuntu 16.04 / ROS Kinetic:

```bash
cd <catkin_workspace>
source /opt/ros/kinetic/setup.bash
catkin_make --pkg huace_m7xx_ros_driver
source devel/setup.bash
```

Recommended verification order:

1. Set a valid period for the new message through a launch parameter or the
   `/OUTMSG` service
2. Start the driver
3. Check that the topic exists:

```bash
rostopic list
```

4. Check that the message is being published:

```bash
rostopic echo /<your_topic_name>
```

### 11. Recommended Development Order

Extend the driver in this order:

1. Add the NMEA text message
2. Add the GMF structured message
3. Add one message at a time and complete compilation, serial, and topic tests

### 12. Minimal Checklist

Before submitting a new message, verify at least:

- The low-level `Message` class has been added
- The class has been registered with `MessageClass`
- The node header declares the publisher, parameters, and callback
- `src/huace_m7xx_ros_driver_node.cpp` creates the publisher, requests output,
  and registers the callback
- NMEA conversion is under `src/Parser/NMEA/`, and GMF conversion is under
  `src/Parser/GMF/`
- Parameter loading, period handling, and service handling are in the
  corresponding `Parameter/` and `Service/` files
- `launch/application.launch` exposes the startup parameters
- `OUTMSG` / `OFFMSG` service support has been considered for the new message
- New `.cpp` files are included in `CMakeLists.txt`
- If a structured message is required, `msg/`, `add_message_files(...)`, and
  `generate_messages(...)` have been updated
- `catkin_make --pkg huace_m7xx_ros_driver` has completed successfully
- Actual data has been verified with `rostopic echo`
