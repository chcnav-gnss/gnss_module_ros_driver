#ifndef __HUACE_M7XX_DRIVER_MESSAGE_GMF_H__
#define __HUACE_M7XX_DRIVER_MESSAGE_GMF_H__

#include "message.h"

namespace huace_m7xx_ros_driver
{
namespace message
{
namespace gmf
{
class Gmf : public Message
{
public:
    typedef enum _TIME_STATUS_E : uint8_t
    {
        TIME_STATUS_UNKNOWN            =  0,   /**< Time validity is unknown */
        TIME_STATUS_APPROXIMATE        =  1,   /**< Time is set approximately */
        TIME_STATUS_COARSEADJUSTING    =  2,   /**< Time is approaching coarse precision */
        TIME_STATUS_COARSE             =  3,   /**< This time is valid to coarse precision */
        TIME_STATUS_COARSESTEERING     =  4,   /**< Time is coarse set and is being steered */
        TIME_STATUS_FREEWHEELING       =  5,   /**< Position is lost and the range bias cannot be calculated */
        TIME_STATUS_FINEADJUSTING      =  6,   /**< Time is adjusting to fine precision */
        TIME_STATUS_FINE               =  7,   /**< Time has fine precision */
        TIME_STATUS_FINEBACKUPSTEERING =  8,   /**< Time is fine set and is being steered by the backup system */
        TIME_STATUS_FINESTEERING       =  9,   /**< Time is fine set and is being steered */
        TIME_STATUS_SATTIME            = 10,   /**< Time from satellite. Only used in logs containing satellite data such as ephemeris and almanac */
        TIME_STATUS_MAX                = 0xFF
    } TIME_STATUS_E;

    typedef enum _SATELLITE_SYSTEM_E: uint8_t
    {
        SATELLITE_SYSTEM_GPS     = 0,
        SATELLITE_SYSTEM_GLONASS = 1,
        SATELLITE_SYSTEM_GALLIEO = 2,
        SATELLITE_SYSTEM_SBAS    = 3,
        SATELLITE_SYSTEM_QZSS    = 4,
        SATELLITE_SYSTEM_BDS     = 5,
        SATELLITE_SYSTEM_NAVIC   = 6,
        SATELLITE_SYSTEM_OTHER   = 7,

        SATELLITE_SYSTEM_MAX     = 0xFF,
    } SATELLITE_SYSTEM_E;

    typedef enum _SOLUTION_STATUS_E : uint8_t
    {
        SOLUTION_STATUS_SOL_COMPUTED      = 0,      /**< Solution computed */
        SOLUTION_STATUS_INSUFFICIENT_OBS  = 1,      /**< Insufficient observations */
        SOLUTION_STATUS_NO_CONVERGENCE    = 2,      /**< No convergence */
        SOLUTION_STATUS_SINGULARITY       = 3,      /**< Singularity at parameters matrix */
        SOLUTION_STATUS_COV_TRACE         = 4,      /**< Covariance trace exceeds maximum (trace > 1000 m) */
        SOLUTION_STATUS_VARIANCE          = 5,      /**< Variance exceeds limits */
        SOLUTION_STATUS_PENDING           = 6,      /**< When a SETFIXPOS position command is entered, the receiver computes its own position and determines if the fixed position is valid */
        SOLUTION_STATUS_INVALID_FIX       = 7,      /**< The fixed position, entered using the SETFIXPOS position command, is not valid */
        SOLUTION_STATUS_RESIDUALS         = 8,      /**< Residuals are too large */
        SOLUTION_STATUS_INTEGRITY_WARNING = 9,      /**< Large residuals make position unreliable */

        SOLUTION_STATUS_NUM               = 10,
        SOLUTION_STATUS_MAX               = 0xFF,
    } SOLUTION_STATUS_E;

    typedef enum _POSITION_OR_VELOCITY_TYPE_E : uint8_t
    {
        POSITION_OR_VELOCITY_TYPE_NONE             = 0,                     /**< No solution */
        POSITION_OR_VELOCITY_TYPE_SINGLE           = 1,                     /**< Solution calculated using only data supplied by the GNSS satellites */
        POSITION_OR_VELOCITY_TYPE_SPPDIFF          = 2,                     /**< Solution calculated using pseudorange differential (DGPS, DGNSS) corrections */
                                                                            /**< Reserved 3 */
        POSITION_OR_VELOCITY_TYPE_NARROW_INT       = 4,                     /**< Multi-frequency RTK solution with carrier phase ambiguities resolved to narrow-lane integers */
        POSITION_OR_VELOCITY_TYPE_NARROW_FLOAT     = 5,                     /**< Multi-frequency RTK solution with unresolved, float carrier phase ambiguities */
                                                                            /**< Reserved 6 */
        POSITION_OR_VELOCITY_TYPE_FIXEDPOS         = 7,                     /**< Position has been fixed by the SETFIXPOS position command or by position averaging. */
                                                                            /**< Reserved 8 */
        POSITION_OR_VELOCITY_TYPE_SBAS             = 9,                     /**< Solution calculated using corrections from an SBAS satellite */
        POSITION_OR_VELOCITY_TYPE_PPP              = 10,                    /**< Converged B2b or HAS or MADOCA PPP solution */
        POSITION_OR_VELOCITY_TYPE_PPP_FIXED        = 11,                    /**< Converged PointSky or CLAS PPP solution */
        POSITION_OR_VELOCITY_TYPE_PPP_CONVERGING   = 12,                    /**< Converging PPP solution */
        POSITION_OR_VELOCITY_TYPE_WIDE_INT         = 13,                    /**< Multi-frequency RTK solution with carrier phase ambiguities resolved to wide-lane integers */
        POSITION_OR_VELOCITY_TYPE_L1_INT           = 14,                    /**< Single-frequency RTK solution with carrier phase ambiguities resolved to integers */
        POSITION_OR_VELOCITY_TYPE_L1_FLOAT         = 15,                    /**< Single-frequency RTK solution with unresolved, float carrier phase ambiguities */
        POSITION_OR_VELOCITY_TYPE_DOPPLER_VELOCITY = 16,                    /**< Velocity computed using instantaneous Doppler */
                                                                            /**< Reserved 17-19 */
        POSITION_OR_VELOCITY_TYPE_INS              = 20,                    /**< INS positioning solution */
        POSITION_OR_VELOCITY_TYPE_INS_SBAS         = 21,                    /**< INS position, where the last applied position update used a GNSS solution computed using corrections from an SBAS solution */
        POSITION_OR_VELOCITY_TYPE_INS_SPP          = 22,                    /**< INS position, where the last applied position update used a single point GNSS(SINGLE) solution */
        POSITION_OR_VELOCITY_TYPE_INS_SPPDIFF      = 23,                    /**< INS position, where the last applied position update used a pseudorange differential GNSS (SPPDIFF) solution */
        POSITION_OR_VELOCITY_TYPE_INS_RTKFLOAT     = 24,                    /**< INS position, where the last applied position update used a floating ambiguity RTK (L1_FLOAT or NARROW_FLOAT) solution */
        POSITION_OR_VELOCITY_TYPE_INS_RTKFIXED     = 25,                    /**< INS position, where the last applied position update used a fixed integer ambiguity RTK (L1_INT, WIDE_INT or NARROW_INT) solution */
        POSITION_OR_VELOCITY_TYPE_INS_PPP          = 26,                    /**< INS position, where the last applied position update used B2b or HAS or MADOCA ppp converged solution */
        POSITION_OR_VELOCITY_TYPE_INS_PPPFIXED     = 27,                    /**< INS position, where the last applied position update used PointSky or CLAS ppp converged solution */

        POSITION_OR_VELOCITY_TYPE_NUM              = 28,
        POSITION_OR_VELOCITY_TYPE_MAX              = 0xFF,
    } POSITION_OR_VELOCITY_TYPE_E;

    typedef struct _BINARY_HEADER_T {
        uint8_t   Sync[3];              /**< Head Sync */
        uint8_t   CpuIdle;
        uint16_t  MsgID;
        uint8_t   SequenceNum;
        uint8_t   MsgAttribute;
        uint16_t  Reserved;
        uint16_t  BodyLen;
        uint8_t   Port;
        uint8_t   TimeStatus;
        uint16_t  GpsWeekNum;
        uint32_t  Milliseconds;
        uint32_t  FirmwareVersion;
        uint32_t  ReceiverStatus;
    } __attribute__((packed)) BINARY_HEADER_T;

    Gmf(void);
    virtual ~Gmf();
    virtual PARSE_RESULT_E ParseBinary(std::shared_ptr<IOInterface> io);

    static bool IsValidMessageName(std::string msg_name);
    static bool IsSlaveAntMessage(const std::vector<uint8_t> &msg);
    static const std::unordered_set<std::string> supported_message_name_set;
}; /* class Gmf */
} /* namespace gmf */
} /* namespace message */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_MESSAGE_GMF_H__ */