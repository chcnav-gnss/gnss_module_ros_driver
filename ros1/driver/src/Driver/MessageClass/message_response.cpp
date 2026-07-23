#include "huace_m7xx_ros_driver/Driver/MessageClass/message_response.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{
    MessageClassResponse::MSG_BASIC_INFO_T MessageClassResponse::msg_basic_info = {};

    const std::vector<uint8_t> MessageClassResponse::MESSAGE_IDENTIFIER = { '>', 'R', 'E', 'S', 'P', 'O', 'N', 'S', 'E', ',' };

    MessageClassResponse::MessageClassResponse(void)
    {
        Log::Trace("MessageClassResponse Ctor");
        this->supported_msgs.insert({message::Response::MESSAGE_ID, std::make_shared<message::Response>()});
    }

    MessageClassResponse::~MessageClassResponse()
    {
        Log::Trace("MessageClassResponse Dtor");
        this->supported_msgs.clear();
    }

    message::Message::PARSE_RESULT_E MessageClassResponse::ProcessMessages(std::shared_ptr<IOInterface> io) {
        Log::Trace("Processing Response Message.");
        message::Message::PARSE_RESULT_E pres = message::Message::PARSE_RESULT_UNFINDHEADER;
        for (auto it = this->supported_msgs.begin(); it != this->supported_msgs.end(); ++it) {
            pres = it->second->ParseResponse(io);
            if (message::Message::PARSE_RESULT_UNFINDHEADER != pres) {
                break;
            }
        }
        if (message::Message::PARSE_RESULT_UNFINDHEADER == pres) {
            return message::Message::PARSE_RESULT_CONTENTERROR;
        }
        return pres;
    }
}
