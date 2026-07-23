#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include <algorithm>
#include <cctype>

#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"
#include "huace_m7xx_ros_driver/Driver/Message/gmf.h"
#include "huace_m7xx_ros_driver/Driver/Message/nmea.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{
namespace
{

std::string ToLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return value;
}

std::string NormalizeBestpMessageName(const std::string &message_name)
{
    if (message_name.size() >= 2 &&
        message_name.compare(message_name.size() - 2, 2, "_1") == 0) {
        std::string converted = message_name;
        converted.insert(converted.end() - 2, 'B');
        return converted;
    }
    return message_name + "B";
}

}  // namespace

bool HuaceM7xxDriverNode::ServiceCallbackOffMsg(
    OffMsg::Request &request, OffMsg::Response &response)
{
    if (request.message == "NOTSET") {
        command::Offmsg command;
        if (Ok != GetCommandResponseErrorCode(command, &response.error_code)) {
            response.error_code = ROS_DRIVER_ERROR_CODE;
        }
        return true;
    }

    const std::string lower_name = ToLower(request.message);
    bool success = false;
    int32_t error_code = message::Response::INVALID_PARAMETER;

    if (message::NMEA::IsValidMessageName(lower_name)) {
        success = SetPeriodParameter<nmea_msgs::Sentence>(
            message::GGA::MESSAGE_NAME, "0", publisher_nmea_gga_, "NMEA/GGA",
            &error_code);
        if (success) {
            nmea_gga_period_ = "0";
        }
    } else if (message::gmf::Gmf::IsValidMessageName(
                   NormalizeBestpMessageName(request.message))) {
        success = SetPeriodParameter<BESTP>(
            message::gmf::BestP::MESSAGE_NAME, "0", publisher_gmf_bestp_, "BESTP",
            &error_code);
        if (success) {
            gmf_bestp_period_ = "0";
        }
    }

    response.error_code = success ? message::Response::OK : error_code;
    return true;
}

bool HuaceM7xxDriverNode::ServiceCallbackOutMsg(
    OutMsg::Request &request, OutMsg::Response &response)
{
    const std::string lower_name = ToLower(request.message);
    bool success = false;
    int32_t error_code = message::Response::INVALID_PARAMETER;

    if (message::NMEA::IsValidMessageName(lower_name)) {
        success = SetPeriodParameter<nmea_msgs::Sentence>(
            message::GGA::MESSAGE_NAME, request.period,
            publisher_nmea_gga_, "NMEA/GGA", &error_code);
        if (success) {
            nmea_gga_period_ = request.period;
        }
    } else if (message::gmf::Gmf::IsValidMessageName(
                   NormalizeBestpMessageName(request.message))) {
        success = SetPeriodParameter<BESTP>(
            message::gmf::BestP::MESSAGE_NAME, request.period,
            publisher_gmf_bestp_, "BESTP", &error_code);
        if (success) {
            gmf_bestp_period_ = request.period;
        }
    }

    response.error_code = success ? message::Response::OK : error_code;
    return true;
}

}  // namespace huace_m7xx_ros_driver
