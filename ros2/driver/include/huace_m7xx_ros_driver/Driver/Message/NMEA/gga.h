#ifndef __HUACE_M7XX_DRIVER_MESSAGE_GGA_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_GGA_H__

#include "../nmea.h"

namespace huace_m7xx_ros_driver
{
namespace message
{

class GGA : public NMEA
{
public:
    GGA(void);
    ~GGA();
    inline uint16_t GetMessageID(void) {
        return this->MESSAGE_ID;
    }
    inline const std::string GetMessageName(void) {
        return this->MESSAGE_NAME;
    }

    static constexpr uint16_t MESSAGE_ID = NMEA::NMEA_MESSAGE_GGA;
    static const std::string MESSAGE_NAME;

}; /* class GGA */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_MESSAGE_GGA_H__ */
