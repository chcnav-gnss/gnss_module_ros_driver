#ifndef __HUACE_M7XX_DRIVER_MESSAGE_PORT_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_PORT_H__

#include "message.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
class Port : public Message
{
public:
    Port(void);
    virtual ~Port();

    inline uint16_t GetMessageID(void) {
        return this->MESSAGE_ID;
    }
    inline const std::string GetMessageName(void) {
        return this->MESSAGE_NAME;
    }

    virtual PARSE_RESULT_E ParsePort(std::shared_ptr<IOInterface> io);

    static constexpr uint16_t MESSAGE_ID = 0;
    static const std::string MESSAGE_NAME;
};
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_PORT_H__ */
