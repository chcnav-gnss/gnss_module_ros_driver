cmake_minimum_required(VERSION 3.8)
project(huace_m7xx_ros_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wno-pedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(nmea_msgs REQUIRED)
find_package(rtcm_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MessageHeader.msg"
  "msg/SolutionStatus.msg"
  "msg/PositionOrVelocityType.msg"
  "msg/ExtendedSolutionStatus.msg"
  "msg/GpsGloSigMask.msg"
  "msg/BeiDouSigMask.msg"
  "msg/GalSigMask.msg"
  "msg/BESTP.msg"
  "srv/OffMsg.srv"
  "srv/OutMsg.srv"
  DEPENDENCIES std_msgs
)

include_directories(include)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")

set(DRIVER_SOURCES
  src/Driver/Command/command.cpp
  src/Driver/Command/config_com.cpp
  src/Driver/Command/offmsg.cpp
  src/Driver/Command/outmsg.cpp
  src/Driver/IOInterface/buffer.cpp
  src/Driver/IOInterface/fileport.cpp
  src/Driver/IOInterface/iointerface.cpp
  src/Driver/IOInterface/serialport.cpp
  src/Driver/Message/GMF/bestp.cpp
  src/Driver/Message/NMEA/gga.cpp
  src/Driver/Message/gmf.cpp
  src/Driver/Message/message.cpp
  src/Driver/Message/nmea.cpp
  src/Driver/Message/outconfig.cpp
  src/Driver/Message/port.cpp
  src/Driver/Message/response.cpp
  src/Driver/MessageClass/message_class.cpp
  src/Driver/MessageClass/message_gmf.cpp
  src/Driver/MessageClass/message_nmea.cpp
  src/Driver/MessageClass/message_outconfig.cpp
  src/Driver/MessageClass/message_port.cpp
  src/Driver/MessageClass/message_response.cpp
  src/Driver/huace_m7xx_common.cpp
  src/Driver/huace_m7xx_ros_driver.cpp
  src/Driver/huace_m7xx_log.cpp
)

set(driver_core_target ${PROJECT_NAME}_core)
add_library(${driver_core_target} SHARED
  ${DRIVER_SOURCES}
)
set_target_properties(${driver_core_target} PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME}
)
install(TARGETS ${driver_core_target}
  LIBRARY DESTINATION lib
)

set(node_target ${PROJECT_NAME}_node)
add_library(${node_target} SHARED
  src/huace_m7xx_ros_driver_node.cpp
  src/Service/service_init.cpp
  src/Service/service_callback.cpp
  src/Parameter/parameter_init.cpp
  src/Parameter/parameter_callback.cpp
  src/Parser/NMEA/gga.cpp
  src/Parser/GMF/bestp.cpp
  src/Parser/response.cpp
)

target_link_libraries(${node_target}
  ${driver_core_target}
  pthread
  "${cpp_typesupport_target}"
)

target_include_directories(${node_target} PUBLIC
  include
)

ament_target_dependencies(${node_target}
  rclcpp
  rclcpp_components
  std_msgs
  nmea_msgs
  rtcm_msgs
)

rclcpp_components_register_nodes(${node_target} "huace_m7xx_ros_driver::HuaceM7xxDriverNode")

add_executable(${PROJECT_NAME}_app
  src/huace_m7xx_ros_driver_main.cpp
)

target_link_libraries(${PROJECT_NAME}_app
  ${node_target}
)

target_include_directories(${PROJECT_NAME}_app PUBLIC
  include
)

ament_target_dependencies(${PROJECT_NAME}_app
  rclcpp
)

install(TARGETS ${node_target} ${PROJECT_NAME}_app
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch
)

install(
  PROGRAMS script/startup.sh DESTINATION share/${PROJECT_NAME}/script
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
