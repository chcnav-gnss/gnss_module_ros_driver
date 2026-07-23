#include "huace_m7xx_ros_driver/Driver/MessageClass/message_class.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_common.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/IOInterface/iointerface.h"

namespace huace_m7xx_ros_driver {

    MessageClass::MessageClass()
    {
        Log::Trace("MessageClass Ctor.");
    }
    MessageClass::~MessageClass()
    {
        Log::Trace("MessageClass Dtor.");
    }

    Result MessageClass::RegisterMessage(std::shared_ptr<message::Message> msg) {
        if (0 != this->supported_msgs.count(msg->GetMessageID())) {
            return Err;
        }
        this->supported_msgs[msg->GetMessageID()] = msg;
        return Ok;
    }

    Result MessageClass::RegisterMessage(std::shared_ptr<message::Message> msg, const std::string &cb_name, message::Message::Callback cb, void *user) {
        if (0 != this->supported_msgs.count(msg->GetMessageID())) {
            return Err;
        }
        if (Ok != msg->RegisterCallbackFn(cb_name, cb, user)) {
            Log::Warn("Register callback function failed.");
        }
        this->supported_msgs[msg->GetMessageID()] = msg;
        return Ok;
    }

    Result MessageClass::UnregisterMessage(uint16_t msg_id) {
        if (0 == this->supported_msgs.count(msg_id)) {
            return Err;
        }
        this->supported_msgs.erase(msg_id);
        return Ok;
    }

    Result MessageClass::RegisterMessageCallback(uint16_t msg_id, const std::string &cb_name, message::Message::Callback cb, void *user) {
        if (0 == this->supported_msgs.count(msg_id)) {
            return Err;
        }
        return this->supported_msgs[msg_id]->RegisterCallbackFn(cb_name, cb, user);
    }

    Result MessageClass::UnregisterMessageCallback(uint16_t msg_id, const std::string &cb_name) {
        if (0 == this->supported_msgs.count(msg_id)) {
            return Err;
        }
        return this->supported_msgs[msg_id]->UnregisterCallbackFn(cb_name);
    }

    MessageClass::MATCH_RESULT_E MessageClass::IsMessageClassIdentifierMatch(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> identifier = this->GetMessageClassIdentifier();
        std::vector<uint8_t> prefix = io->Peek(identifier.size());
        if (identifier.size() != prefix.size()) {
            return MessageClass::MATCH_RESULT_DATATRUNCATE;
        }
        if (std::equal(identifier.begin(), identifier.end(), prefix.begin(), prefix.end())){
            return MessageClass::MATCH_RESULT_EXACT_MATCH;
        }
        return MessageClass::MATCH_RESULT_NOT_MATCH;
    }

    message::Message::PARSE_RESULT_E MessageClass::ProcessMessages(std::shared_ptr<IOInterface> io) {
        (void) io;
        return message::Message::PARSE_RESULT_UNFINDHEADER;
    }

} /* namespace huace_m7xx_ros_driver */
