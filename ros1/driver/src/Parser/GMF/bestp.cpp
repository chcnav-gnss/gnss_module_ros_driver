#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include <cstring>

#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"

namespace huace_m7xx_ros_driver
{

Result HuaceM7xxDriverNode::ProcessGmfMessageBESTP(
    const std::vector<uint8_t> &message_data, void *user)
{
    (void)user;
    const size_t expected_size =
        sizeof(message::gmf::BestP::BESTP_MESSAGE_T) + 4;
    if (message_data.size() != expected_size) {
        ROS_WARN("BESTP size mismatch: expected %zu bytes, received %zu bytes.",
                 expected_size, message_data.size());
        return Err;
    }

    message::gmf::BestP::BESTP_MESSAGE_T raw_message{};
    std::memcpy(&raw_message, message_data.data(), sizeof(raw_message));
    const auto &raw_header = raw_message.Header;

    BESTP output;
    ProcessStdMsgHeader(&output.header);

    output.raw_msg_header.message_id = raw_header.MsgID;
    output.raw_msg_header.antenna_index = raw_header.MsgAttribute & 0x01;
    output.raw_msg_header.sequence_num = raw_header.SequenceNum;
    output.raw_msg_header.cpu_idle = raw_header.CpuIdle;
    output.raw_msg_header.gps_time_status = raw_header.TimeStatus;
    output.raw_msg_header.gps_week_num = raw_header.GpsWeekNum;
    output.raw_msg_header.gps_milliseconds = raw_header.Milliseconds;
    output.raw_msg_header.firmware_version = raw_header.FirmwareVersion;
    output.raw_msg_header.receiver_status = raw_header.ReceiverStatus;

    output.pos_sol_status.status = raw_message.PosSolStatus;
    output.pos_type.type = raw_message.PosType;
    output.datum_id = raw_message.DatumID;
    output.lat = raw_message.Lat;
    output.lon = raw_message.Lon;
    output.hgt = raw_message.Hgt;
    output.undulation = raw_message.Undulation;
    output.lat_std = raw_message.LatStd;
    output.lon_std = raw_message.LonStd;
    output.hgt_std = raw_message.HgtStd;
    output.diff_age = raw_message.DiffAge;
    output.sol_age = raw_message.SolAge;
    output.track_svs = raw_message.TrackSVs;
    output.sol_svs = raw_message.SolSVs;
    output.sol_muti_svs = raw_message.SolMutiSVs;
    output.sol_l1_svs = raw_message.SolL1SVs;
    output.ext_sol_status.status = raw_message.ExtSolStatus;
    output.gps_glo_sig_mask.mask = raw_message.GpsGloSigMask;
    output.bd_sig_mask.mask = raw_message.BdSigMask;
    output.gal_sig_mask.mask = raw_message.GalSigMask;
    output.station_id = raw_message.StationID;

    if (publisher_gmf_bestp_) {
        publisher_gmf_bestp_.publish(output);
    }
    return Ok;
}

}  // namespace huace_m7xx_ros_driver
