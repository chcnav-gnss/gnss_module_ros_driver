#include "huace_m7xx_ros_driver/Driver/MessageClass/message_outconfig.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/Message/outconfig.h"

namespace huace_m7xx_ros_driver
{
    MessageClassOutConfig::MSG_BASIC_INFO_T MessageClassOutConfig::msg_basic_info = {};

    const std::vector<uint8_t> MessageClassOutConfig::MESSAGE_IDENTIFIER = {'<', '\t', '>'};

    MessageClassOutConfig::MessageClassOutConfig(void)
    {
        Log::Trace("MessageClassOutConfig Ctor");
        this->supported_msgs.insert({message::OutConfig::MESSAGE_ID, std::make_shared<message::OutConfig>()});
    }

    MessageClassOutConfig::~MessageClassOutConfig()
    {
        Log::Trace("MessageClassNMEA Dtor");
        this->supported_msgs.clear();
    }

    message::Message::PARSE_RESULT_E MessageClassOutConfig::ProcessMessages(std::shared_ptr<IOInterface> io) {
        Log::Trace("Processing OutConfig Message.");
        message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
        for (auto it = this->supported_msgs.begin(); it != this->supported_msgs.end(); ++it) {
            pres = it->second->ParseOutConfig(io);
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
