cmake_minimum_required(VERSION 3.0.2)
project(huace_m7xx_ros_driver)

add_compile_options(-Wall -Wextra -Wno-pedantic)

if(CMAKE_VERSION VERSION_LESS "3.8")
  add_compile_options(-std=c++1z)
else()
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  nmea_msgs
  rtcm_msgs
  message_generation
)

add_message_files(
  FILES
  MessageHeader.msg
  SolutionStatus.msg
  PositionOrVelocityType.msg
  ExtendedSolutionStatus.msg
  GpsGloSigMask.msg
  BeiDouSigMask.msg
  GalSigMask.msg
  BESTP.msg
)

add_service_files(
  FILES
  OffMsg.srv
  OutMsg.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES huace_m7xx_ros_driver_core
  CATKIN_DEPENDS roscpp std_msgs nmea_msgs rtcm_msgs message_runtime
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

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
  src/kinetic_compat.cpp
)

set(driver_core_target ${PROJECT_NAME}_core)
add_library(${driver_core_target}
  ${DRIVER_SOURCES}
)

set(node_target ${PROJECT_NAME}_node)
add_executable(${node_target}
  src/huace_m7xx_ros_driver_node.cpp
  src/Parameter/parameter_init.cpp
  src/Parameter/parameter_callback.cpp
  src/Parser/NMEA/gga.cpp
  src/Parser/GMF/bestp.cpp
  src/Parser/response.cpp
  src/Service/service_init.cpp
  src/Service/service_callback.cpp
  src/huace_m7xx_ros_driver_main.cpp
)

target_link_libraries(${node_target}
  ${driver_core_target}
  pthread
  ${catkin_LIBRARIES}
)

add_dependencies(${node_target}
  ${${PROJECT_NAME}_EXPORTED_TARGETS}
  ${catkin_EXPORTED_TARGETS}
)

install(TARGETS ${driver_core_target} ${node_target}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
  DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
)

install(
  DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)

install(
  PROGRAMS script/startup.sh
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/script
)
