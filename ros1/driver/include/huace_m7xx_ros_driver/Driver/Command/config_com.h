#ifndef __HUACE_M7XX_DRIVER_COMMAND_CONFIGCOM_H__
#define __HUACE_M7XX_DRIVER_COMMAND_CONFIGCOM_H__

#include "command.h"
#include "outmsg.h"

namespace huace_m7xx_ros_driver
{
namespace command
{
class ConfigCom : public Command
{
public:
    using Command::GetCmdString;
    enum COM_PARITY_E {
        COM_PARITY_NONE,
        COM_PARITY_ODD,
        COM_PARITY_EVEN,
        COM_PARITY_MAX,
    };
    ConfigCom(void) = delete;
    ConfigCom(uint32_t baudrate);
    ConfigCom(uint32_t baudrate, COM_PARITY_E parity);
    ConfigCom(uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits);
    ConfigCom(uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits, uint8_t stop_bits);
    ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate);
    ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity);
    ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits);
    ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits, uint8_t stop_bits);
    ~ConfigCom();

    inline const std::string GetCommandName() override final {
        return this->COMMAND_NAME;
    }

    Result GetCmdString(std::string &out_cmd);

    static bool IsValidBaudrate(uint32_t baudrate);

    static std::string GetComParityString(COM_PARITY_E parity);
    static COM_PARITY_E GetComParityEnum(std::string str);
    static bool IsValidComParity(COM_PARITY_E parity);
    static bool IsValidComParity(std::string str);

    static bool IsValidDataBits(uint8_t data_bits);
    static bool IsValidStopBits(uint8_t stop_bits);
    
    static const std::string COMMAND_NAME;
    static const std::map<COM_PARITY_E, std::string> parity_map;
    static constexpr uint32_t UNSPECIFIED_BAUDRATE = 0;
    static constexpr COM_PARITY_E UNSPECIFIED_PARITY = COM_PARITY_MAX;
    static constexpr uint8_t UNSPECIFIED_DATA_BITS = 0;
    static constexpr uint8_t UNSPECIFIED_STOP_BITS = 0;

private:
    Outmsg::OUTMSG_PORT_E port;
    uint32_t baudrate;
    COM_PARITY_E parity;
    uint8_t data_bits;
    uint8_t stop_bits;

}; /* class ConfigCom */
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_COMMAND_CONFIGCOM_H__ */
