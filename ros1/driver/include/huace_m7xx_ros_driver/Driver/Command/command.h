#ifndef __HUACE_M7XX_DRIVER_COMMAND_H__
#define __HUACE_M7XX_DRIVER_COMMAND_H__

#include "../huace_m7xx_common.h"
#include <memory>
#include <unordered_set>
#include <map>

namespace huace_m7xx_ros_driver
{
namespace command
{
class Command
{
public:
    enum COMMAND_RESULT_E {
        COMMAND_RESULT_OK,
        COMMAND_RESULT_CHECKSUM_ERROR,
        COMMAND_RESULT_UNSPECIFIED_COMMAND,
        COMMAND_RESULT_UNSPECIFIED_PARAMETER,
        COMMAND_RESULT_PARAMETER_EXCEED_LIMIT,
        COMMAND_RESULT_PARAMETER_NUMBERS_ERROR,
        COMMAND_RESULT_UNKNOWN_ERROR,
    };

    enum SWITCH_MODE_E : uint8_t {
        SWITCH_MODE_DISABLE = 0,
        SWITCH_MODE_ENABLE  = 1,
        SWITCH_MODE_MAX     = 0xFF
    };

    Command(void);
    virtual ~Command();
    virtual inline const std::string GetCommandName(void) = 0;

    virtual Result GetCmdString(std::string &out_cmd) = 0;
    virtual Result GetCmdString(const std::shared_ptr<std::string> out_cmd);

    static std::string GetSwitchModeString(SWITCH_MODE_E enable);
    static SWITCH_MODE_E GetSwitchModeEnum(std::string str);
    static bool IsValidSwitchMode(SWITCH_MODE_E enable);
    static bool IsValidSwitchMode(std::string str);

    static bool IsValidCmdName(std::string cmd_name);

    static Result AppendCheckXOR(std::string &cmd);
    static Result AppendCheckXOR(const std::shared_ptr<std::string> cmd);

    static const std::unordered_set<std::string> supported_command_name_set;
    static const std::map<SWITCH_MODE_E, std::string> switch_mode_map;

    static constexpr SWITCH_MODE_E UNSPECIFIED_ENABLE = SWITCH_MODE_MAX;
    static const std::string UNSPECIFIED_COMMAND_NAME;

    static constexpr char COMMAND_HEADER  = '>';
    static constexpr char PARAM_DELIMITER = ',';
    static constexpr char CHECK_DELIMITER = '*';
    static constexpr char CARRIAGE_RETURN = '\r';
    static constexpr char LINE_FEED       = '\n';
}; /* class Command */
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_COMMAND_H__ */
