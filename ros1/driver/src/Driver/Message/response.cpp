#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_response.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
    const std::string Response::MESSAGE_NAME = "RESPONSE";
    const std::map<Response::ERROR_CODE_E, std::string> Response::error_code_table = {
        {Response::OK,                      "OK"},
        {Response::CHECKSUM_ERROR,          "CHECKSUM_ERROR"},
        {Response::INVALID_COMMAND,         "INVALID_COMMAND"},
        {Response::INVALID_PARAMETER,       "INVALID_PARAMETER"},
        {Response::PARAMETER_EXCEED_LIMIT,  "PARAMETER_EXCEED_LIMIT"},
        {Response::PARAMETER_NUMBERS_ERROR, "PARAMETER_NUMBERS_ERROR"},
        {Response::UNKNOWN_ERROR,           "UNKNOWN_ERROR"},
        {Response::CMD_INPROGRESS,          "CMD_INPROGRESS"},
        {Response::CMD_EXEC_FAIL,           "CMD_EXEC_FAIL"},
        {Response::DEVICE_BUSY,             "DEVICE_BUSY"},
        {Response::UNAUTH,                  "UNAUTH"},
        {Response::SIGNATURE_INVALID,       "SIGNATURE_INVALID"},
        {Response::NO_AUTHCODE,             "NO_AUTHCODE"},
        {Response::GPS_TIME_INVALID,        "GPS_TIME_INVALID"},
        {Response::AUTHCODE_USED,           "AUTHCODE_USED"},
        {Response::FIRMWARE_NOT_MATCH,      "FIRMWARE_NOT_MATCH"},
        {Response::TRIAL_UNAVAILABLE,       "TRIAL_UNAVAILABLE"},
        {Response::MODULE_TYPE_NOT_MATCH,   "MODULE_TYPE_NOT_MATCH"},
        {Response::AUTH_SERVICE_INVALID,    "AUTH_SERVICE_INVALID"},
        {Response::AUTH_ALREADY_ACTIVATED,  "AUTH_ALREADY_ACTIVATED"},
        {Response::CONFIG_LOCKED,           "CONFIG_LOCKED"}
    };

    Response::Response(void)
        : Message()
    {
        Log::Trace("Response Ctor.");
    }

    Response::~Response()
    {
        Log::Trace("Response Dtor.");
    }

    Message::PARSE_RESULT_E Response::ParseResponse(std::shared_ptr<IOInterface> io) {
        std::vector<uint8_t> line = io->PeekLine();
        /** Do not find \r\n */
        if (0 == line.size()) {
            return Message::PARSE_RESULT_DATATRUNCATE;
        }

        /** Determine whether it is a RESPONSE message */
        std::string msg_name = this->GetMessageName();
        if (!std::equal(line.begin() + 1, line.begin() + 1 + msg_name.length(), msg_name.begin(), msg_name.end())) {
            return Message::PARSE_RESULT_UNFINDHEADER;
        }

        std::string cmd_name = Response::GetCommandName(line);

        /** Calculate checksum */
        std::vector<uint8_t> check = CalculateXOR_S(line, 1, line.size() - 6); /**< 6 == sizeof('>' + '*' + checksum + \r\n) */
        if (!std::equal(line.end() - 4, line.end() - 2, check.begin(), check.end())) {
            std::copy(cmd_name.begin(), cmd_name.end(), std::begin(MessageClassResponse::msg_basic_info.CommandName));
            MessageClassResponse::msg_basic_info.CommandName[cmd_name.length()] = '\0';
            MessageClassResponse::msg_basic_info.UnpassCheckFlag = true;
            return Message::PARSE_RESULT_UNPASSCHECK;
        }

        Log::Info("Received " + msg_name + " Message: " + std::string(line.begin(), line.end() - 2) +
            ", " + std::to_string(line.size()) + " Bytes In Total.");

        /** Execute callback */
        this->ExecuteCallbacks(line);

        /** Drop Message */
        if (line.size() != io->Drop(line.size())) {
            Log::Error("Failed to drop " + std::to_string(line.size()) + " bytes.");
            return Message::PARSE_RESULT_DATATRUNCATE;
        }
        return Message::PARSE_RESULT_PARSEDONE;
    }

    Response::ERROR_CODE_E Response::GetErrorCode(const std::vector<uint8_t> &response) {
        for (auto it = Response::error_code_table.begin(); it != error_code_table.end(); ++it) {
            if (std::equal(it->second.begin(), it->second.end(), response.begin() + Response::MESSAGE_NAME.length() + 2)) { /**< '>' + RESPONSE + ',' */
                return it->first;
            }
        }
        Log::Error("Unknown error code.");
        return Response::UNKNOWN_ERROR;
    }

    std::string Response::GetErrorCodeString(const std::vector<uint8_t> &response) {
        Response::ERROR_CODE_E err = Response::GetErrorCode(response);
        return Response::error_code_table.at(err);
    }

    std::string Response::GetErrorCodeString(Response::ERROR_CODE_E error_code) {
        return Response::error_code_table.at(error_code);
    }

    std::string Response::GetCommandName(const std::vector<uint8_t> &response) {
        auto it_start = std::find(response.begin() + 1, response.end(), '>');
        if (it_start == response.end()) {
            return std::string();
        }
        auto start = it_start + 1;  // indicate the character bebind '>'
        if (start >= response.end()) {
            return std::string();
        }
        auto it_end = std::find_if(start, response.end(),
            [](uint8_t c) {
                return c == ',' || c == '*';
            }
        );
        if (it_end == response.end()) {
            return std::string();
        }
        return std::string(start, it_end);
    }
} /* namespace messsage */
} /* namespace huace_m7xx_ros_driver */
