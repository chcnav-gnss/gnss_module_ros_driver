#ifndef __HUACE_M7XX_DRIVER_MESSAGE_CLASS_NMEA_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_CLASS_NMEA_H__

#include "message_class.h"

namespace huace_m7xx_ros_driver
{

class MessageClassNMEA : public MessageClass
{
public:
    typedef struct _MSG_BASIC_INFO_T
    {
        char MsgName[32];
        bool SlaveAnt;
        bool UnpassCheckFlag;
    } MSG_BASIC_INFO_T;

    MessageClassNMEA(void);
    ~MessageClassNMEA();

    inline uint32_t GetMessageClassID(void) override final {
        return MessageClassNMEA::MESSAGE_CLASS_ID;
    }

    inline std::vector<uint8_t> GetMessageClassIdentifier(void) override final {
        return MessageClassNMEA::MESSAGE_IDENTIFIER;
    }

    MessageClass::MATCH_RESULT_E IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io) override final;
    message::Message::PARSE_RESULT_E ProcessMessages(std::shared_ptr<IOInterface> io) override final;

    static MSG_BASIC_INFO_T msg_basic_info;

    static constexpr uint32_t MESSAGE_CLASS_ID = MESSAGE_CLASS_NMEA;
    static const std::vector<uint8_t> MESSAGE_IDENTIFIER;
    static const std::vector<std::vector<uint8_t>> MESSAGE_IDENTIFIERS;
}; /* class MessageClassNMEA */
    
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_CLASS_NMEA_H__ */