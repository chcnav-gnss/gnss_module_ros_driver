#ifndef __HUACE_M7XX_DRIVER_MESSAGE_OUTCONFIG_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_OUTCONFIG_H__

#include "message.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
class OutConfig : public Message
{
public:
    OutConfig(void);
    virtual ~OutConfig();

    inline uint16_t GetMessageID(void) {
        return this->MESSAGE_ID;
    }
    inline const std::string GetMessageName(void) {
        return this->MESSAGE_NAME;
    }

    PARSE_RESULT_E ParseOutConfig(std::shared_ptr<IOInterface> io) override final;

    static std::string GetConfigName(const std::vector<uint8_t> &msg);

    static constexpr uint16_t MESSAGE_ID = 0;
    static const std::string MESSAGE_NAME;
};
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_OUTCONFIG_H__ */
