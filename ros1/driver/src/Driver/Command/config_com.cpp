#include "huace_m7xx_ros_driver/Driver/Command/config_com.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include <algorithm>

namespace huace_m7xx_ros_driver
{
namespace command
{
    const std::string ConfigCom::COMMAND_NAME = "CONFIGCOM";
    const std::map<ConfigCom::COM_PARITY_E, std::string> ConfigCom::parity_map {
        {COM_PARITY_NONE, "NONE"},
        {COM_PARITY_ODD,  "ODD"},
        {COM_PARITY_EVEN, "EVEN"},
    };

    ConfigCom::ConfigCom(uint32_t baudrate)
        : port(Outmsg::OUTMSG_PORT_SELF), baudrate(baudrate), parity(UNSPECIFIED_PARITY), data_bits(UNSPECIFIED_DATA_BITS), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(uint32_t baudrate, COM_PARITY_E parity)
        : port(Outmsg::OUTMSG_PORT_SELF), baudrate(baudrate), parity(parity), data_bits(UNSPECIFIED_DATA_BITS), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits)
        : port(Outmsg::OUTMSG_PORT_SELF), baudrate(baudrate), parity(parity), data_bits(data_bits), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits, uint8_t stop_bits)
        : port(Outmsg::OUTMSG_PORT_SELF), baudrate(baudrate), parity(parity), data_bits(data_bits), stop_bits(stop_bits)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate)
        : port(port), baudrate(baudrate), parity(UNSPECIFIED_PARITY), data_bits(UNSPECIFIED_DATA_BITS), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity)
        : port(port), baudrate(baudrate), parity(parity), data_bits(UNSPECIFIED_DATA_BITS), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits)
        : port(port), baudrate(baudrate), parity(parity), data_bits(data_bits), stop_bits(UNSPECIFIED_STOP_BITS)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::ConfigCom(Outmsg::OUTMSG_PORT_E port, uint32_t baudrate, COM_PARITY_E parity, uint8_t data_bits, uint8_t stop_bits)
        : port(port), baudrate(baudrate), parity(parity), data_bits(data_bits), stop_bits(stop_bits)
    {
        Log::Trace("ConfigCom Ctor.");
    }

    ConfigCom::~ConfigCom()
    {
        Log::Trace("ConfigCom Dtor.");
    }

    Result ConfigCom::GetCmdString(std::string &out_cmd)
    {
        if (UNSPECIFIED_BAUDRATE == this->baudrate || !this->IsValidBaudrate(this->baudrate)) {
            return Err;
        }

        out_cmd.push_back(COMMAND_HEADER);
        out_cmd.append(COMMAND_NAME);

        if (Outmsg::UNSPECIFIED_PORT != this->port) {
            if (!Outmsg::IsValidPort(this->port)) {
                return Err;
            }
            out_cmd.push_back(PARAM_DELIMITER);
            out_cmd.append(Outmsg::GetOutmsgPortString(this->port));
        }

        out_cmd.push_back(PARAM_DELIMITER);
        out_cmd.append(std::to_string(this->baudrate));

        if (UNSPECIFIED_PARITY != this->parity) {
            if (!this->IsValidComParity(this->parity)) {
                return Err;
            }
            out_cmd.push_back(PARAM_DELIMITER);
            out_cmd.append(GetComParityString(this->parity));

            if (UNSPECIFIED_DATA_BITS != this->data_bits) {
                if (!this->IsValidDataBits(this->data_bits)) {
                    return Err;
                }
                out_cmd.push_back(PARAM_DELIMITER);
                out_cmd.append(std::to_string(this->data_bits));

                if (UNSPECIFIED_STOP_BITS != this->stop_bits) {
                    if (!this->IsValidStopBits(this->stop_bits)) {
                        return Err;
                    }
                    out_cmd.push_back(PARAM_DELIMITER);
                    out_cmd.append(std::to_string(this->stop_bits));
                }
            }
        }
        return Command::AppendCheckXOR(out_cmd);
    }

    bool ConfigCom::IsValidBaudrate(uint32_t baudrate) {
        return baudrate == 9600 || baudrate == 19200 || baudrate == 115200 || 
               baudrate == 460800 || baudrate == 921600 || baudrate == 1500000 || baudrate == 3000000;
    }

    std::string ConfigCom::GetComParityString(COM_PARITY_E parity) {
        return ConfigCom::parity_map.at(parity);
    }

    ConfigCom::COM_PARITY_E ConfigCom::GetComParityEnum(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        for(auto it = ConfigCom::parity_map.begin(); it != ConfigCom::parity_map.end(); ++it) {
            if (it->second == str) {
                return it->first;
            }
        }
        return ConfigCom::COM_PARITY_MAX;
    }

    bool ConfigCom::IsValidComParity(COM_PARITY_E parity) {
        return 0 != ConfigCom::parity_map.count(parity);
    }

    bool ConfigCom::IsValidComParity(std::string str) {
        return ConfigCom::COM_PARITY_MAX != ConfigCom::GetComParityEnum(str);
    }

    bool ConfigCom::IsValidDataBits(uint8_t data_bits) {
        return data_bits == 5 || data_bits == 6 || data_bits == 7 || data_bits == 8;
    }

    bool ConfigCom::IsValidStopBits(uint8_t stop_bits) {
        return stop_bits == 1 || stop_bits == 2;
    }

} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
