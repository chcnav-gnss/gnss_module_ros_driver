#include "huace_m7xx_ros_driver/Driver/Message/port.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    const std::string Port::MESSAGE_NAME = "PORT";

    Port::Port(void)
        : Message()
    {
        Log::Trace("Port Ctor.");
    }

    Port::~Port()
    {
        Log::Trace("Port Dtor.");
    }

    Message::PARSE_RESULT_E Port::ParsePort(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> line = io->PeekLine();
        if (0 == line.size()) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        std::string msg_name = this->GetMessageName();
        Log::Debug("Received " + msg_name + " Message, " + std::to_string(line.size()) + " Bytes In Total.");

        this->ExecuteCallbacks(line);

        if (line.size() != io->Drop(line.size())) {
            Log::Error("Failed to drop " + std::to_string(line.size()) + " bytes.");
            return Message::PARSE_RESULT_DATATRUNCATE;
        }
        return Message::PARSE_RESULT_PARSEDONE;
    }
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */
