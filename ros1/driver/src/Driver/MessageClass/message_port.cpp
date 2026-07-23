#include "huace_m7xx_ros_driver/Driver/MessageClass/message_port.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/Message/port.h"

namespace huace_m7xx_ros_driver
{
    const std::vector<uint8_t> MessageClassPort::MESSAGE_IDENTIFIER = { '[', '*', '*', '*', '*', ']' };
    const std::vector<std::vector<uint8_t>> MessageClassPort::MESSAGE_IDENTIFIERS = {
        {'[', 'C', 'O', 'M', '1', ']'},
        {'[', 'C', 'O', 'M', '2', ']'},
        {'[', 'C', 'O', 'M', '3', ']'},
    };

    MessageClassPort::MessageClassPort(void) {
        Log::Trace("MessageClassPort Ctor.");
        this->supported_msgs.insert({message::Port::MESSAGE_ID, std::make_shared<message::Port>()});
    }

    MessageClassPort::~MessageClassPort() {
        Log::Trace("MessageClassPort Dtor.");
        this->supported_msgs.clear();
    }

    MessageClass::MATCH_RESULT_E MessageClassPort::IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> buffer = io->Peek(this->MESSAGE_IDENTIFIER.size());
        if (this->MESSAGE_IDENTIFIER.size() != buffer.size()) {
            return MessageClass::MATCH_RESULT_DATATRUNCATE;
        }
        for (auto it = MESSAGE_IDENTIFIERS.begin(); it != MESSAGE_IDENTIFIERS.end(); ++it) {
            if (std::equal(it->begin(), it->end(), buffer.begin(), buffer.end())) {
                return MessageClass::MATCH_RESULT_EXACT_MATCH;
            }
        }
        return MessageClass::MATCH_RESULT_NOT_MATCH;
    }

    message::Message::PARSE_RESULT_E MessageClassPort::ProcessMessages(std::shared_ptr<IOInterface> io) {
        Log::Trace("Processing Port Message");
        message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
        for (auto it = this->supported_msgs.begin(); it != this->supported_msgs.end(); ++it) {
            pres = it->second->ParsePort(io);
            if (message::Message::PARSE_RESULT_UNFINDHEADER != pres) {
                break;
            }
        }
        if (message::Message::PARSE_RESULT_UNFINDHEADER == pres) {
            return message::Message::PARSE_RESULT_CONTENTERROR;
        }
        return pres;
    }

} /* namespace huace_m7xx_ros_driver */
