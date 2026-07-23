#include "huace_m7xx_ros_driver/Driver/Message/nmea.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_nmea.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    NMEA::NMEA(void)
        : Message() { }

    NMEA::~NMEA() { }

    const std::unordered_set<std::string> NMEA::supported_message_name_set = {"GGA"};

    Result NMEA::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName() + "_1", msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result NMEA::GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) {
        command::Outmsg cmd = command::Outmsg(this->GetMessageName() + "_1", msg_period);
        return cmd.GetCmdString(out_cmd);
    }

    Result NMEA::GetCancelSlaveAntMessageCmd(std::string &out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName() + "_1");
        return cmd.GetCmdString(out_cmd);
    }

    Result NMEA::GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd) {
        command::Offmsg cmd = command::Offmsg(this->GetMessageName() + "_1");
        return cmd.GetCmdString(out_cmd);
    }

    Message::PARSE_RESULT_E NMEA::ParseNMEA(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> line = io->PeekLine();
        /** Do not find \r\n */
        if (0 == line.size()) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        /** Determine whether it is a NMEA message */
        std::string msg_name = this->GetMessageName();
        if (!std::equal(line.begin() + 3, line.begin() + 6, msg_name.begin(), msg_name.end())) {
            return Message::PARSE_RESULT_UNFINDHEADER;
        }

        Log::Debug("Received " + msg_name + " Message, " + std::to_string(line.size()) + " Bytes In Total.");

        /** Calculate checksum */
        std::vector<uint8_t> check = CalculateXOR_S(line, 1, line.size() - 6); /**< 6 == '$' + '*' + checksum + \r\n */
        if (!std::equal(line.end() - 4, line.end() - 2, check.begin(), check.end())) {
            std::copy(msg_name.begin(), msg_name.end(), std::begin(MessageClassNMEA::msg_basic_info.MsgName));
            MessageClassNMEA::msg_basic_info.MsgName[msg_name.length()] = '\0';
            MessageClassNMEA::msg_basic_info.SlaveAnt = NMEA::IsSlaveAntMessage(line);
            MessageClassNMEA::msg_basic_info.UnpassCheckFlag = true;
            return Message::PARSE_RESULT_UNPASSCHECK;
        }

        /** Execute callback */
        this->ExecuteCallbacks(line);

        /** Drop Message */
        if (line.size() != io->Drop(line.size())) {
            Log::Error("Failed to drop " + std::to_string(line.size()) + " bytes.");
            return Message::PARSE_RESULT_DATATRUNCATE;
        }
        return Message::PARSE_RESULT_PARSEDONE;
    }

    bool NMEA::IsValidMessageName(std::string msg_name) {
        std::transform(msg_name.begin(), msg_name.end(), msg_name.begin(), ::toupper);
        return 0 != NMEA::supported_message_name_set.count(msg_name);
    }

    bool NMEA::IsSlaveAntMessage(const std::vector<uint8_t> &msg) {
        return std::equal(msg.begin() + 6, msg.begin() + 8, "_1");
    }
} /* namespace messsage */
} /* namespace huace_m7xx_ros_driver */
