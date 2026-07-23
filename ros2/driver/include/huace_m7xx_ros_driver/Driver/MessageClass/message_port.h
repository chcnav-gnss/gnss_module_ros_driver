#ifndef __HUACE_M7XX_DRIVER_MESSAGE_CLASS_PORT_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_CLASS_PORT_H__

#include "message_class.h"

namespace huace_m7xx_ros_driver
{

class MessageClassPort : public MessageClass
{
public:
    MessageClassPort(void);
    ~MessageClassPort();

    inline uint32_t GetMessageClassID(void) override final {
        return MessageClassPort::MESSAGE_CLASS_ID;
    }

    inline std::vector<uint8_t> GetMessageClassIdentifier(void) override final {
        return MessageClassPort::MESSAGE_IDENTIFIER;
    }

    MessageClass::MATCH_RESULT_E IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io) override final;
    message::Message::PARSE_RESULT_E ProcessMessages(std::shared_ptr<IOInterface> io) override final;

    static constexpr uint32_t MESSAGE_CLASS_ID = MESSAGE_CLASS_PORT;
    static const std::vector<uint8_t> MESSAGE_IDENTIFIER;
    static const std::vector<std::vector<uint8_t>> MESSAGE_IDENTIFIERS;
};

} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_CLASS_PORT_H__ */
