#ifndef __HUACE_M7XX_DRIVER_MESSAGE_CLASS_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_CLASS_H__

#include "../Message/message.h"
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace huace_m7xx_ros_driver
{

class MessageClass
{
public:
    enum MESSAGE_CLASS_E {
        MESSAGE_CLASS_GMF         = 0,
        MESSAGE_CLASS_NMEA,
        MESSAGE_CLASS_RESPONSE,
        MESSAGE_CLASS_OUTCONFIG,
        MESSAGE_CLASS_PORT,
        MESSAGE_CLASS_MAX,
    };

    enum MATCH_RESULT_E {
        MATCH_RESULT_EXACT_MATCH,
        MATCH_RESULT_DATATRUNCATE,
        MATCH_RESULT_NOT_MATCH,
    };

    MessageClass(void);
    virtual ~MessageClass();
    virtual inline uint32_t GetMessageClassID(void) = 0;
    virtual inline std::vector<uint8_t> GetMessageClassIdentifier(void) = 0;

    virtual Result RegisterMessage(std::shared_ptr<message::Message> msg);
    virtual Result RegisterMessage(std::shared_ptr<message::Message> msg, const std::string &cb_name, message::Message::Callback cb, void *user);
    virtual Result UnregisterMessage(uint16_t msg_id);

    virtual Result RegisterMessageCallback(uint16_t msg_id, const std::string &cb_name, message::Message::Callback cb, void *user);
    virtual Result UnregisterMessageCallback(uint16_t msg_id, const std::string &cb_name);

    virtual MATCH_RESULT_E IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io);
    virtual message::Message::PARSE_RESULT_E ProcessMessages(std::shared_ptr<IOInterface> io);

    std::unordered_map<uint16_t, std::shared_ptr<message::Message>> supported_msgs;
}; /* class MessageClass */

} /* namespace huace_m7xx_ros_driver */


#endif /* __HUACE_M7XX_DRIVER_MESSAGE_CLASS_H__ */

