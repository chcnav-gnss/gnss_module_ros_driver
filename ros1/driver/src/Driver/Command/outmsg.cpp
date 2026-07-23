#include "huace_m7xx_ros_driver/Driver/Command/outmsg.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include "huace_m7xx_ros_driver/Driver/Message/nmea.h"
#include "huace_m7xx_ros_driver/Driver/Message/gmf.h"
#include <algorithm>

namespace huace_m7xx_ros_driver
{
namespace command
{
    const std::string Outmsg::COMMAND_NAME = "OUTMSG";
    const std::map<Outmsg::OUTMSG_PORT_E, std::string> Outmsg::port_map {
        {OUTMSG_PORT_COM1, "COM1"},
        {OUTMSG_PORT_COM2, "COM2"},
        {OUTMSG_PORT_COM3, "COM3"},
    };
    const std::map<Outmsg::OUTMSG_PERIOD_E, std::string> Outmsg::period_map {
        {OUTMSG_PERIOD_ONSCHEDULE,  "ONSCHEDULE"},
        {OUTMSG_PERIOD_ONCHANGED,   "ONCHANGED" },
        {OUTMSG_PERIOD_ONNEW,       "ONNEW"     },
        {OUTMSG_PERIOD_ONCE,        "ONCE"      },
        {OUTMSG_PERIOD_ONMARK,      "ONMARK"    },
        {OUTMSG_PERIOD_20HZ,        "0.05"      },
        {OUTMSG_PERIOD_10HZ,        "0.1"       },
        {OUTMSG_PERIOD_5HZ,         "0.2"       },
        {OUTMSG_PERIOD_2HZ,         "0.5"       },
        {OUTMSG_PERIOD_1HZ,         "1"         },
        {OUTMSG_PERIOD_2S,          "2"         },
        {OUTMSG_PERIOD_5S,          "5"         },
        {OUTMSG_PERIOD_10S,         "10"        },
        {OUTMSG_PERIOD_15S,         "15"        },
        {OUTMSG_PERIOD_20S,         "20"        },
        {OUTMSG_PERIOD_30S,         "30"        },
        {OUTMSG_PERIOD_60S,         "60"        },
        {OUTMSG_PERIOD_120S,        "120"       }
    };
    const std::string Outmsg::UNSPECIFIED_MESSAGE_NAME = "";

    Outmsg::Outmsg(std::string msg_name, OUTMSG_PERIOD_E msg_period)
        : port(UNSPECIFIED_PORT), msg_name(msg_name), period(msg_period)
    {
        Log::Trace("Outmsg Ctor.");
    }

    Outmsg::Outmsg(OUTMSG_PORT_E port, std::string msg_name, OUTMSG_PERIOD_E msg_period)
        : port(port), msg_name(msg_name), period(msg_period)
    {
        Log::Trace("Outmsg Ctor.");
    }

    Outmsg::~Outmsg()
    {
        Log::Trace("Outmsg Dtor.");
    }

    Result Outmsg::GetCmdString(std::string &out_cmd) {
        if (UNSPECIFIED_MESSAGE_NAME == this->msg_name || !this->IsValidMessageName(this->msg_name) ||
            UNSPECIFIED_PERIOD == this->period || !this->IsValidPeriod(this->period)) {
            return Err;
        }

        out_cmd.push_back(COMMAND_HEADER);
        out_cmd.append(COMMAND_NAME);

        if (UNSPECIFIED_PORT != this->port) {
            if (!this->IsValidPort(this->port)) {
                return Err;
            }
            out_cmd.push_back(PARAM_DELIMITER);
            out_cmd.append(GetOutmsgPortString(port));
        }

        out_cmd.push_back(PARAM_DELIMITER);
        out_cmd.append(this->msg_name);

        out_cmd.push_back(PARAM_DELIMITER);
        out_cmd.append(GetOutmsgPeriodString(this->period));

        return Command::AppendCheckXOR(out_cmd);
    }

    std::string Outmsg::GetOutmsgPortString(OUTMSG_PORT_E port) {
        return Outmsg::port_map.at(port);
    }

    Outmsg::OUTMSG_PORT_E Outmsg::GetOutmsgPortEnum(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        for(auto it = Outmsg::port_map.begin(); it != Outmsg::port_map.end(); ++it) {
            if (it->second == str) {
                return it->first;
            }
        }
        return Outmsg::OUTMSG_PORT_SELF;
    }

    bool Outmsg::IsValidPort(OUTMSG_PORT_E port) {
        return 0 != Outmsg::port_map.count(port);
    }

    bool Outmsg::IsValidPort(std::string str) {
        return OUTMSG_PORT_SELF != Outmsg::GetOutmsgPortEnum(str);
    }

    bool Outmsg::IsValidMessageName(std::string msg_name) {
        std::transform(msg_name.begin(), msg_name.end(), msg_name.begin(), ::toupper);
        return message::NMEA::IsValidMessageName(msg_name) ||
               message::gmf::Gmf::IsValidMessageName(msg_name) ||
               message::Message::IsValidMessageGroup(msg_name);
    }

    std::string Outmsg::GetOutmsgPeriodString(OUTMSG_PERIOD_E period) {
        return Outmsg::period_map.at(period);
    }

    Outmsg::OUTMSG_PERIOD_E Outmsg::GetOutmsgPeriodEnum(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        for(auto it = Outmsg::period_map.begin(); it != Outmsg::period_map.end(); ++it) {
            if (it->second == str) {
                return it->first;
            }
        }
        return Outmsg::OUTMSG_PERIOD_MAX;
    }

    bool Outmsg::IsValidPeriod(OUTMSG_PERIOD_E period) {
        return 0 != Outmsg::period_map.count(period);
    }

    bool Outmsg::IsValidPeriod(std::string str) {
        return Outmsg::OUTMSG_PERIOD_MAX != Outmsg::GetOutmsgPeriodEnum(str);
    }
    
} /* namespace command */
} /* namespace huace_m7xx_ros_driver */
