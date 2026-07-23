#include "huace_m7xx_ros_driver/Driver/MessageClass/message_nmea.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

#include "huace_m7xx_ros_driver/Driver/Message/nmea.h"
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"

namespace huace_m7xx_ros_driver
{

MessageClassNMEA::MSG_BASIC_INFO_T MessageClassNMEA::msg_basic_info = {};

const std::vector<uint8_t> MessageClassNMEA::MESSAGE_IDENTIFIER = {'$', 'G', '*'};
const std::vector<std::vector<uint8_t>> MessageClassNMEA::MESSAGE_IDENTIFIERS = {
    {'$', 'G', 'P'},
    {'$', 'G', 'L'},
    {'$', 'G', 'A'},
    {'$', 'G', 'B'},
    {'$', 'G', 'Q'},
    {'$', 'G', 'D'},
    {'$', 'G', 'I'},
    {'$', 'G', 'N'}
};

MessageClassNMEA::MessageClassNMEA(void)
{
    Log::Trace("MessageClassNMEA Ctor");
    this->supported_msgs.insert({message::NMEA::NMEA_MESSAGE_GGA, std::make_shared<message::GGA>()});
}

MessageClassNMEA::~MessageClassNMEA()
{
    Log::Trace("MessageClassNMEA Dtor");
    this->supported_msgs.clear();
}

MessageClass::MATCH_RESULT_E MessageClassNMEA::IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io)
{
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

message::Message::PARSE_RESULT_E MessageClassNMEA::ProcessMessages(std::shared_ptr<IOInterface> io)
{
    Log::Trace("Processing NMEA Message.");
    message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
    for (auto it = this->supported_msgs.begin(); it != this->supported_msgs.end(); ++it) {
        pres = it->second->ParseNMEA(io);
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
