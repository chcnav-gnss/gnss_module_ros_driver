#include "huace_m7xx_ros_driver/Driver/MessageClass/message_gmf.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"

namespace huace_m7xx_ros_driver
{

MessageClassGmf::MSG_BASIC_INFO_T MessageClassGmf::msg_basic_info = {};

const std::vector<uint8_t> MessageClassGmf::MESSAGE_IDENTIFIER = {0xAA, 0x44, 0x72};

MessageClassGmf::MessageClassGmf(void)
{
    Log::Trace("MessageClassGmf Ctor.");
    this->supported_msgs.insert({message::gmf::BestP::MESSAGE_ID, std::make_shared<message::gmf::BestP>()});
}

MessageClassGmf::~MessageClassGmf()
{
    Log::Trace("MessageClassGmf Dtor.");
    this->supported_msgs.clear();
}

message::Message::PARSE_RESULT_E MessageClassGmf::ProcessMessages(std::shared_ptr<IOInterface> io)
{
    Log::Trace("Processing Gmf Message");
    message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
    for (auto it = this->supported_msgs.begin(); it != this->supported_msgs.end(); ++it) {
        pres = it->second->ParseBinary(io);
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
