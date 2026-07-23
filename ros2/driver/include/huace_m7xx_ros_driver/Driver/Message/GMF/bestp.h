#ifndef __HUACE_M7XX_DRIVER_GMF_BESTP_H__
#define __HUACE_M7XX_DRIVER_GMF_BESTP_H__

#include "../gmf.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
namespace gmf
{
class BestP : public Gmf 
{
public:
    typedef struct _BESTP_MESSAGE_T
    {
        BINARY_HEADER_T Header;                /**< Binary message header */
        SOLUTION_STATUS_E PosSolStatus;        /**< Position Solution Status */
        POSITION_OR_VELOCITY_TYPE_E PosType;   /**< Position Solution Type */
        uint8_t DatumID;                       /**< Datum ID number
                                                    0 = WGS84
                                                    1 = USER */
        uint8_t Reserved1;
        double Lat;                            /**< Latitude (degrees) */
        double Lon;                            /**< Longitude (degrees) */
        double Hgt;                            /**< Height above mean sea level (metres) */
        float Undulation;                      /**< Undulation - the relationship between the geoid and the ellipsoid (m) of the chosen datum
                                                    When using a datum other than WGS84, the undulation value also includes the vertical shift
                                                    due to differences between the datum in use and WGS84. */
        float LatStd;                          /**< Latitude standard deviation (m) */
        float LonStd;                          /**< Longitude standard deviation (m) */
        float HgtStd;                          /**< Height standard deviation (m) */
        float DiffAge;                         /**< Differential age in seconds */
        float SolAge;                          /**< Solution age in seconds */
        uint8_t TrackSVs;                      /**< Number of satellites tracked */
        uint8_t SolSVs;                        /**< Number of satellites used in solution */
        uint8_t SolMutiSVs;                    /**< Number of satellites with multi-frequency signals used in solution */
        uint8_t SolL1SVs;                      /**< Number of satellites with L1/E1/B1 signals used in solution */
        uint16_t ExtSolStatus;                 /**< Pseudorange Iono Correction
                                                    Bit 0-2: 
                                                        0 = Unknown or default Klobuchar model
                                                        1 = Klobuchar Broadcast
                                                        2 = SBAS Broadcast
                                                        3 = Multi-frequency Computed
                                                        4 = SPPDiff Correction
                                                    Bit 3: AtmIntegrity Use Flat
                                                        0 = Not Used
                                                        1 = Used
                                                    Bit 4-7: AtmIntegrity Version
                                                        0 = Version 3.0
                                                        1 = Version 4.0
                                                    Bit 8-13: AtmIntegrity Timeout (0~61s)
                                                    bit 14-15: Reserved; */  
        uint8_t GpsGloSigMask;                 /**< GPS and GLONASS Signal-Used Mask
                                                    Bit  Mask  Description
                                                    0   0x01  GPS L1 used in Solution
                                                    1   0x02  GPS L2 used in Solution
                                                    2   0x04  GPS L5 used in Solution
                                                    3   0x08  Reserved
                                                    4   0x10  GLONASS L1 used in Solution
                                                    5   0x20  GLONASS L2 used in Solution
                                                    6   0x40  GLONASS L3 used in Solution
                                                    7   0x80  Reserved */
        uint8_t BdSigMask;                     /**< BeiDou Signal-Used Mask
                                                    Bit  Mask  Description
                                                     0   0x01  BeiDou B1 used in Solution (B1I )
                                                     1   0x02  BeiDou B2 used in Solution (B2I)
                                                     2   0x04  BeiDou B3 used in Solution (B3I)
                                                     3   0x08  BeiDou B1C used in Solution
                                                     4   0x10  Beidou B2a used in Solution
                                                     5   0x20  Beidou B2b used in Solution
                                                    6-7  0xC0  Reserved */
        uint8_t GalSigMask;                    /**< Galileo Signal-Used Mask
                                                    Bit       Mask  Description
                                                     0   0x01  Galileo E1 used in Solution
                                                     1   0x02  Galileo E5a used in Solution
                                                     2   0x04  Galileo E5b used in Solution
                                                     3   0x08  Galileo ALTBOC used in Solution
                                                     4   0x10  Galileo E6 used in Solution (E6B and E6C)
                                                    5-7  0xE0  Reserved */
        uint8_t Reserved2;
        uint16_t StationID;                    /**< Base station ID */
    } __attribute__((packed)) BESTP_MESSAGE_T;
    BestP(void);
    ~BestP();
    inline uint16_t GetMessageID() override final {
        return this->MESSAGE_ID;
    }
    inline const std::string GetMessageName() override final {
        return this->MESSAGE_NAME;
    }

    Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::string &out_cmd) override final;
    Result GetRequestSlaveAntMessageCmd(command::Outmsg::OUTMSG_PERIOD_E msg_period, std::shared_ptr<std::string> out_cmd) override final;
    Result GetCancelSlaveAntMessageCmd(std::string &out_cmd) override final;
    Result GetCancelSlaveAntMessageCmd(std::shared_ptr<std::string> out_cmd) override final;

    static constexpr uint16_t MESSAGE_ID = 3020;
    static const std::string MESSAGE_NAME;
}; /* class BestP */
} /* namespace gmf */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_GMF_BESTP_H__ */