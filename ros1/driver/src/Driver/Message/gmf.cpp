#include "huace_m7xx_ros_driver/Driver/Message/gmf.h"
#include <cstring>
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_gmf.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
namespace gmf
{
    Gmf::Gmf(void)
        : Message() { }

    Gmf::~Gmf() { }

    const std::unordered_set<std::string> Gmf::supported_message_name_set = {
        "BESTPB"
    };

    Message::PARSE_RESULT_E Gmf::ParseBinary(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> header = io->Peek(sizeof(BINARY_HEADER_T));
        if (header.size() != sizeof(BINARY_HEADER_T)) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        BINARY_HEADER_T raw_header{};
        std::memcpy(&raw_header, header.data(), sizeof(raw_header));

        /** Determine whether it is a Gmf message */
        if (this->GetMessageID() != raw_header.MsgID) {
            return Message::PARSE_RESULT_UNFINDHEADER;
        }

        /** Peek Complete message */
        uint32_t expect_size = sizeof(BINARY_HEADER_T) + raw_header.BodyLen + 4; /**< CRC 4 byte */
        std::vector<uint8_t> message = io->Peek(expect_size);
        if (message.size() != expect_size) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        std::string msg_name = this->GetMessageName();
        Log::Debug("Received " + msg_name + " Message, " + std::to_string(expect_size) + " Bytes In Total.");

        /** Calculate CRC */
        uint32_t received_crc = 0;
        std::memcpy(&received_crc, message.data() + expect_size - 4, sizeof(received_crc));
        if (CalculateCRC32(message, 0, expect_size - 4) != received_crc) {
            MessageClassGmf::msg_basic_info.MsgID = raw_header.MsgID;
            std::copy(msg_name.begin(), msg_name.end(), std::begin(MessageClassGmf::msg_basic_info.MsgName));
            MessageClassGmf::msg_basic_info.MsgName[msg_name.length()] = '\0';
            MessageClassGmf::msg_basic_info.SlaveAnt = Gmf::IsSlaveAntMessage(message);
            MessageClassGmf::msg_basic_info.UnpassCheckFlag = true;
            return Message::PARSE_RESULT_UNPASSCHECK;
        }

        /** Execute callback */
        this->ExecuteCallbacks(message);

        /** Drop Message */
        if (expect_size != io->Drop(expect_size)) {
            Log::Error("Failed to drop " + std::to_string(expect_size) + " bytes.");
            return Message::PARSE_RESULT_DATATRUNCATE;
        }
        return Message::PARSE_RESULT_PARSEDONE;
    }

    bool Gmf::IsValidMessageName(std::string msg_name) {
        std::transform(msg_name.begin(), msg_name.end(), msg_name.begin(), ::toupper);
        return 0 != Gmf::supported_message_name_set.count(msg_name);
    }

    bool Gmf::IsSlaveAntMessage(const std::vector<uint8_t> &msg) {
        if (msg.size() < sizeof(BINARY_HEADER_T)) {
            return false;
        }

        BINARY_HEADER_T raw_header{};
        std::memcpy(&raw_header, msg.data(), sizeof(raw_header));
        return (0 != (raw_header.MsgAttribute & 0x01));
    }
} /* namespace gmf */
} /* namespace messsage */
} /* namespace huace_m7xx_ros_driver */
