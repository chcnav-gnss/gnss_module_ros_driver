#include "huace_m7xx_ros_driver/Driver/Message/outconfig.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_outconfig.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    const std::string OutConfig::MESSAGE_NAME = "OUTCONFIG";

    OutConfig::OutConfig(void)
        : Message()
    {
        Log::Trace("OutConfig Ctor.");
    }

    OutConfig::~OutConfig()
    {
        Log::Trace("OutConfig Dtor.");
    }

    Message::PARSE_RESULT_E OutConfig::ParseOutConfig(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> line = io->PeekLine();
        if (0 == line.size()) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        std::string msg_name = this->GetMessageName();
        std::string cfg_name = OutConfig::GetConfigName(line);
        Log::Debug("Received " + cfg_name + " " + msg_name + " Message, " + std::to_string(line.size()) + " Bytes In Total.");

        std::vector<uint8_t> check = CalculateXOR_S(line, 3, line.size() - 8);
        if (!std::equal(line.end() - 4, line.end() - 2, check.begin(), check.end())) {
            std::copy(cfg_name.begin(), cfg_name.end(), std::begin(MessageClassOutConfig::msg_basic_info.ConfigName));
            MessageClassOutConfig::msg_basic_info.ConfigName[cfg_name.length()] = '\0';
            MessageClassOutConfig::msg_basic_info.UnpassCheckFlag = true;
            return Message::PARSE_RESULT_UNPASSCHECK;
        }

        this->ExecuteCallbacks(line);

        if (line.size() != io->Drop(line.size())) {
            Log::Error("Failed to drop " + std::to_string(line.size()) + " bytes.");
            return Message::PARSE_RESULT_DATATRUNCATE;
        }
        return Message::PARSE_RESULT_PARSEDONE;
    }

    std::string OutConfig::GetConfigName(const std::vector<uint8_t> &msg) {
        for (auto it = msg.begin(); it != msg.end(); ++it) {
            if (*it == ',') {
                return std::string(msg.begin() + 3, it);
            }
        }
        return std::string();
    }
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */
