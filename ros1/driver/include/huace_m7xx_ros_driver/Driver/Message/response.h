#ifndef __HUACE_M7XX_DRIVER_MESSAGE_RESPONSE_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_RESPONSE_H__

#include "message.h"
#include <map>

namespace huace_m7xx_ros_driver
{
namespace message
{
class Response : public Message
{
public:

    enum ERROR_CODE_E : int32_t {
        OK                      =  0,
        CHECKSUM_ERROR          = -1,
        INVALID_COMMAND         = -2,
        INVALID_PARAMETER       = -3,
        PARAMETER_EXCEED_LIMIT  = -4,
        PARAMETER_NUMBERS_ERROR = -5,
        UNKNOWN_ERROR           = -6,
        CMD_INPROGRESS          = -7,
        CMD_EXEC_FAIL           = -8,
        DEVICE_BUSY             = -9,
        UNAUTH                  = -10,
        SIGNATURE_INVALID       = -11,
        NO_AUTHCODE             = -12,
        GPS_TIME_INVALID        = -13,
        AUTHCODE_USED           = -14,
        FIRMWARE_NOT_MATCH      = -15,
        TRIAL_UNAVAILABLE       = -16,
        MODULE_TYPE_NOT_MATCH   = -17,
        AUTH_SERVICE_INVALID    = -18,
        AUTH_ALREADY_ACTIVATED  = -20,
        CONFIG_LOCKED           = -22,
    };

    Response(void);
    virtual ~Response();

    inline uint16_t GetMessageID(void) {
        return this->MESSAGE_ID;
    }
    inline const std::string GetMessageName(void) {
        return this->MESSAGE_NAME;
    }

    PARSE_RESULT_E ParseResponse(std::shared_ptr<IOInterface> io) override final;

    static ERROR_CODE_E GetErrorCode(const std::vector<uint8_t> &response);
    static std::string GetErrorCodeString(const std::vector<uint8_t> &response);
    static std::string GetErrorCodeString(ERROR_CODE_E error_code);
    static std::string GetCommandName(const std::vector<uint8_t> &response);

    static const std::map<ERROR_CODE_E, std::string> error_code_table;
    static constexpr uint16_t MESSAGE_ID = 0; /** Any number is fine */
    static const std::string MESSAGE_NAME;

}; /* class Response */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_RESPONSE_H__ */
