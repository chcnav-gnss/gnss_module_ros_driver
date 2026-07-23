#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"
#include "huace_m7xx_ros_driver/Driver/Message/gmf.h"

namespace huace_m7xx_ros_driver
{
void HuaceM7xxDriverNode::ProcessStdMsgHeader(std_msgs::msg::Header *header)
{
    header->stamp = this->get_clock()->now();
    header->frame_id = this->frame_id_;
}

Result HuaceM7xxDriverNode::ProcessGmfMessageBESTP(const std::vector<uint8_t> &msg, void *user)
{
    RCLCPP_DEBUG(this->get_logger(), "Publishing BESTP Message.");
    (void)user;
    message::gmf::BestP::BESTP_MESSAGE_T *raw_msg = (message::gmf::BestP::BESTP_MESSAGE_T *)msg.data();
    message::gmf::Gmf::BINARY_HEADER_T *raw_msg_header = (message::gmf::Gmf::BINARY_HEADER_T *)msg.data();
    auto pub_msg = std::make_unique<huace_m7xx_ros_driver::msg::BESTP>();

    size_t expect_size = sizeof(message::gmf::BestP::BESTP_MESSAGE_T) + 4;
    if (msg.size() != expect_size) {
        RCLCPP_WARN(this->get_logger(), "Received BESTP Message Not Match Struct Definition, Expect %lu bytes, Received %lu bytes.", expect_size, msg.size());
        return Err;
    }

    this->ProcessStdMsgHeader(&pub_msg->header);

    pub_msg->raw_msg_header.message_id = raw_msg_header->MsgID;
    pub_msg->raw_msg_header.antenna_index = raw_msg_header->MsgAttribute & 0x01;
    pub_msg->raw_msg_header.sequence_num = raw_msg_header->SequenceNum;
    pub_msg->raw_msg_header.cpu_idle = raw_msg_header->CpuIdle;
    pub_msg->raw_msg_header.gps_time_status = raw_msg_header->TimeStatus;
    pub_msg->raw_msg_header.gps_week_num = raw_msg_header->GpsWeekNum;
    pub_msg->raw_msg_header.gps_milliseconds = raw_msg_header->Milliseconds;
    pub_msg->raw_msg_header.firmware_version = raw_msg_header->FirmwareVersion;
    pub_msg->raw_msg_header.receiver_status = raw_msg_header->ReceiverStatus;

    pub_msg->pos_sol_status.status = raw_msg->PosSolStatus;
    pub_msg->pos_type.type = raw_msg->PosType;
    pub_msg->datum_id = raw_msg->DatumID;
    pub_msg->lat = raw_msg->Lat;
    pub_msg->lon = raw_msg->Lon;
    pub_msg->hgt = raw_msg->Hgt;
    pub_msg->undulation = raw_msg->Undulation;
    pub_msg->lat_std = raw_msg->LatStd;
    pub_msg->lon_std = raw_msg->LonStd;
    pub_msg->hgt_std = raw_msg->HgtStd;
    pub_msg->diff_age = raw_msg->DiffAge;
    pub_msg->sol_age = raw_msg->SolAge;
    pub_msg->track_svs = raw_msg->TrackSVs;
    pub_msg->sol_svs = raw_msg->SolSVs;
    pub_msg->sol_muti_svs = raw_msg->SolMutiSVs;
    pub_msg->sol_l1_svs = raw_msg->SolL1SVs;
    pub_msg->ext_sol_status.status = raw_msg->ExtSolStatus;
    pub_msg->gps_glo_sig_mask.mask = raw_msg->GpsGloSigMask;
    pub_msg->bd_sig_mask.mask = raw_msg->BdSigMask;
    pub_msg->gal_sig_mask.mask = raw_msg->GalSigMask;
    pub_msg->station_id = raw_msg->StationID;

    if (this->publisher_gmf_bestp_) {
        this->publisher_gmf_bestp_->publish(std::move(pub_msg));
    }
    return Ok;
}
} /* namespace huace_m7xx_ros_driver */
