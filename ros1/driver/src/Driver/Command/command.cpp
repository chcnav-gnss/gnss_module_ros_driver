#include "huace_m7xx_ros_driver/Driver/Command/command.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include <algorithm>
#include <cctype>

namespace huace_m7xx_ros_driver
{
namespace command
{
    Command::Command(void) { }
    Command::~Command() { }

    const std::unordered_set<std::string> Command::supported_command_name_set = {
        "CONFIGCOM",
        "OFFMSG",
        "OUTMSG"
    };

    const std::map<Command::SWITCH_MODE_E, std::string> Command::switch_mode_map = {
        {SWITCH_MODE_DISABLE, "DISABLE"},
        {SWITCH_MODE_ENABLE,  "ENABLE" }
    };
    const std::string Command::UNSPECIFIED_COMMAND_NAME = "";

    Result Command::GetCmdString(const std::shared_ptr<std::string> out_cmd) {
        if (nullptr == out_cmd) {
            return Err;
        }
        this->GetCmdString(*out_cmd);
        return Ok;
    }

    std::string Command::GetSwitchModeString(SWITCH_MODE_E enable) {
        return Command::switch_mode_map.at(enable);
    }

    Command::SWITCH_MODE_E Command::GetSwitchModeEnum(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        for(auto it = Command::switch_mode_map.begin(); it != Command::switch_mode_map.end(); ++it) {
            if (it->second == str) {
                return it->first;
            }
        }
        return Command::SWITCH_MODE_MAX;
    }

    bool Command::IsValidSwitchMode(SWITCH_MODE_E enable) {
        return 0 != Command::switch_mode_map.count(enable);
    }

    bool Command::IsValidSwitchMode(std::string str) {
        return Command::SWITCH_MODE_MAX != Command::GetSwitchModeEnum(str);
    }

    bool Command::IsValidCmdName(std::string cmd_name) {
        std::transform(cmd_name.begin(), cmd_name.end(), cmd_name.begin(), ::toupper);
        return 0 != Command::supported_command_name_set.count(cmd_name);
    }

    Result Command::AppendCheckXOR(std::string &cmd) {
        if (cmd.length() <= 1 || cmd[0] != COMMAND_HEADER) {
            return Err;
        }
        cmd.push_back(CHECK_DELIMITER);
        cmd.append(CalculateXOR_S(cmd, 1, cmd.length() - 2));
        cmd.push_back(CARRIAGE_RETURN);
        cmd.push_back(LINE_FEED);
        return Ok;
    }
    Result Command::AppendCheckXOR(const std::shared_ptr<std::string> cmd) {
        if (nullptr == cmd) {
            return Err;
        }
        return Command::AppendCheckXOR(*cmd);
    }
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
