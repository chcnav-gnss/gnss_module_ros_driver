#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

namespace huace_m7xx_ros_driver
{

void HuaceM7xxDriverNode::DeclareIntegerParameter(int32_t &param_variable, std::string param_name, int32_t default_value, std::string param_description, bool read_only)
{
    auto param_desc = rcl_interfaces::msg::ParameterDescriptor{};
    param_desc.type = rcl_interfaces::msg::ParameterType::PARAMETER_INTEGER;
    param_desc.name = param_name;
    param_desc.description = param_description;
    param_desc.read_only = read_only;
    this->declare_parameter(param_desc.name, default_value, param_desc);
    this->get_parameter(param_desc.name, param_variable);
}

void HuaceM7xxDriverNode::DeclareStringParameter(std::string &param_variable, std::string param_name, std::string default_value, std::string param_description, bool read_only)
{
    auto param_desc = rcl_interfaces::msg::ParameterDescriptor{};
    param_desc.type = rcl_interfaces::msg::ParameterType::PARAMETER_STRING;
    param_desc.name = param_name;
    param_desc.description = param_description;
    param_desc.read_only = read_only;
    this->declare_parameter(param_desc.name, default_value, param_desc);
    this->get_parameter(param_desc.name, param_variable);
}

void HuaceM7xxDriverNode::DeclarePeriodParameter(std::string &param_variable, std::string param_name, std::string default_value, std::string param_description)
{
    this->DeclareStringParameter(param_variable, param_name, default_value, param_description, false);
}

void HuaceM7xxDriverNode::DeclareParameters(void)
{
    this->DeclareStringParameter(this->com1_, "COM1", "/dev/ttyUSB0", "Uart device.", true);
    this->DeclareIntegerParameter(this->baudrate_, "baudrate", 1500000, "Baudrate of uart interface.", true);
    this->DeclareStringParameter(this->frame_id_, "frame_id", "gnss", "frame_id field of ROS std/header message.", true);

    this->DeclarePeriodParameter(this->nmea_gga_period_, "nmea_gga_period", "5", "Output period of NMEA GGA.");
    this->DeclarePeriodParameter(this->gmf_bestp_period_, "gmf_bestp_period", "0.1", "Output period of GMF BESTP.");
}

} /* namespace huace_m7xx_ros_driver */
