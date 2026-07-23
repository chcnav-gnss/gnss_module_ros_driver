#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    const std::string GGA::MESSAGE_NAME = "GGA";

    GGA::GGA(void)
        : NMEA()
    {
        Log::Trace("GGA Ctor.");
    }

    GGA::~GGA() {
        Log::Trace("GGA Dtor.");
    }

} /* namespace message */
} /* namespace huace_m7xx_ros_driver */
