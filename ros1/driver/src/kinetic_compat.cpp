#include "huace_m7xx_ros_driver/Driver/MessageClass/message_nmea.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_outconfig.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_response.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_gmf.h"
#include "huace_m7xx_ros_driver/Driver/MessageClass/message_port.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include "huace_m7xx_ros_driver/Driver/Message/outconfig.h"
#include "huace_m7xx_ros_driver/Driver/Message/port.h"
#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"
#include <stdint.h>

namespace huace_m7xx_ros_driver
{
constexpr uint32_t MessageClassNMEA::MESSAGE_CLASS_ID;
constexpr uint32_t MessageClassOutConfig::MESSAGE_CLASS_ID;
constexpr uint32_t MessageClassResponse::MESSAGE_CLASS_ID;
constexpr uint32_t MessageClassGmf::MESSAGE_CLASS_ID;
constexpr uint32_t MessageClassPort::MESSAGE_CLASS_ID;
namespace message
{
constexpr uint16_t Response::MESSAGE_ID;
constexpr uint16_t OutConfig::MESSAGE_ID;
constexpr uint16_t Port::MESSAGE_ID;
namespace gmf
{
constexpr uint16_t BestP::MESSAGE_ID;
}
}
}
