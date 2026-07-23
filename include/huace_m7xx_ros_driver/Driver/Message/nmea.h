#ifndef __HUACE_M7XX_DRIVER_MESSAGE_NMEA_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_NMEA_H__

#include "message.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
class NMEA : public Message
{
public:
    enum NMEA_MESSAGE_E {
        NMEA_MESSAGE_GGA = 0,
    };
    NMEA(void);
    virtual ~NMEA();

    virtual Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) override;
    virtual Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) override;
    virtual Result GetCancelSlaveAntMessageCmd(std::string &out_cmd) override;
    virtual Result GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd) override;

    virtual PARSE_RESULT_E ParseNMEA(std::shared_ptr<IOInterface> io);

    static bool IsValidMessageName(std::string msg_name);
    static bool IsSlaveAntMessage(const std::vector<uint8_t> &msg);
    static const std::unordered_set<std::string> supported_message_name_set;
}; /* class NMEA */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_NMEA_H__ */
