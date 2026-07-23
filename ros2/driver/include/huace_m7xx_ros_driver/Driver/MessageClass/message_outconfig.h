#ifndef __HUACE_M7XX_DRIVER_MESSAGE_CLASS_OUTCONFIG_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_CLASS_OUTCONFIG_H__

#include "message_class.h"

namespace huace_m7xx_ros_driver
{

class MessageClassOutConfig : public MessageClass
{
public:
    typedef struct _MSG_BASIC_INFO_T
    {
        char ConfigName[32];
        bool UnpassCheckFlag;
    } MSG_BASIC_INFO_T;

    MessageClassOutConfig(void);
    ~MessageClassOutConfig();

    inline uint32_t GetMessageClassID(void) override final {
        return MessageClassOutConfig::MESSAGE_CLASS_ID;
    }

    inline std::vector<uint8_t> GetMessageClassIdentifier(void) override final {
        return MessageClassOutConfig::MESSAGE_IDENTIFIER;
    }

    message::Message::PARSE_RESULT_E ProcessMessages(std::shared_ptr<IOInterface> io) override final;

    static MSG_BASIC_INFO_T msg_basic_info;

    static constexpr uint32_t MESSAGE_CLASS_ID = MESSAGE_CLASS_OUTCONFIG;
    static const std::vector<uint8_t> MESSAGE_IDENTIFIER;
};
    
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_CLASS_OUTCONFIG_H__ */
