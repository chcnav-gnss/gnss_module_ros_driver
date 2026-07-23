#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include <exception>
#include <functional>
#include <sys/syscall.h>

#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"
#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"

namespace huace_m7xx_ros_driver
{

HuaceM7xxDriverNode::HuaceM7xxDriverNode(
    ros::NodeHandle &nh, ros::NodeHandle &private_nh)
    : nh_(nh), private_nh_(private_nh)
{
    Init();
}

HuaceM7xxDriverNode::~HuaceM7xxDriverNode()
{
    BeginShutdown();
    if (m7xx_thread_.joinable()) {
        m7xx_thread_.join();
    }
    m7xx_.reset();
}

void HuaceM7xxDriverNode::Init()
{
    LoadParameters();

    try {
        m7xx_ = std::make_shared<HuaceM7xxDriver>(
            std::make_shared<SerialPort>("COM1", com1_, 4 * 1024 * 1024),
            static_cast<uint32_t>(baudrate_));
    } catch (const std::exception &error) {
        ROS_FATAL("Failed to initialize COM1 (%s) at %d baud: %s",
                  com1_.c_str(), baudrate_, error.what());
        throw;
    }

    HuaceM7xxDriver::RegisterLogFnTrace(
        [](const std::string &message) { ROS_DEBUG("%s", message.c_str()); });
    HuaceM7xxDriver::RegisterLogFnDebug(
        [](const std::string &message) { ROS_DEBUG("%s", message.c_str()); });
    HuaceM7xxDriver::RegisterLogFnInfo(
        [](const std::string &message) { ROS_INFO("%s", message.c_str()); });
    HuaceM7xxDriver::RegisterLogFnWarn(
        [](const std::string &message) { ROS_WARN("%s", message.c_str()); });
    HuaceM7xxDriver::RegisterLogFnError(
        [](const std::string &message) { ROS_ERROR("%s", message.c_str()); });
    HuaceM7xxDriver::RegisterLogFnFatal(
        [](const std::string &message) { ROS_FATAL("%s", message.c_str()); });

    CreateServices();
    CancelAllMessages();

    CreateNMEAPublisher();
    RegisterNMEACallback();
    RequestNMEA();

    CreateGmfPublisher();
    RegisterGmfCallback();
    RequestGmf();

    subscriber_rtcm_ = nh_.subscribe(
        "RTCM", 10, &HuaceM7xxDriverNode::HandleRtcm, this);

    m7xx_thread_running_ = true;
    m7xx_thread_ = std::thread([this]() {
        const pid_t tid = syscall(SYS_gettid);
        ROS_INFO("[ThreadInfo] [Huace_m7xx_driver] "
                 "[Huace_m7xx_driver_node] M7xx Data Process Thread ID: %d",
                 tid);
        while (m7xx_thread_running_.load() && ros::ok()) {
            m7xx_->ProcessMessages();
        }
    });

    ROS_INFO("M7xx driver initialized: COM1=%s baudrate=%d frame_id=%s.",
             com1_.c_str(), baudrate_, frame_id_.c_str());
}

void HuaceM7xxDriverNode::BeginShutdown()
{
    if (shutdown_started_.exchange(true)) {
        return;
    }

    if (m7xx_) {
        CancelAllMessages();
    }

    m7xx_thread_running_ = false;
    if (m7xx_) {
        m7xx_->RequestStop();
    }
}

void HuaceM7xxDriverNode::CancelAllMessages()
{
    if (!m7xx_) {
        return;
    }

    command::Offmsg command;
    if (Ok != m7xx_->ExecuteCommand(command)) {
        ROS_WARN("Failed to send OFFMSG during initialization or shutdown.");
    }
}

void HuaceM7xxDriverNode::RequestMessage(
    MessageClass::MESSAGE_CLASS_E message_class, uint16_t message_id,
    const std::string &message_name, const std::string &period)
{
    if (!command::Outmsg::IsValidPeriod(period)) {
        if (period != "0") {
            ROS_WARN("Ignoring invalid output period '%s' for %s.",
                     period.c_str(), message_name.c_str());
        }
        return;
    }

    if (Ok != m7xx_->RequestMessage(
                  message_class, message_id,
                  command::Outmsg::GetOutmsgPeriodEnum(period))) {
        ROS_WARN("Failed to request %s output.", message_name.c_str());
    }
}

void HuaceM7xxDriverNode::CreateNMEAPublisher()
{
    CreatePublisher<nmea_msgs::Sentence>(
        publisher_nmea_gga_, nmea_gga_period_, "NMEA/GGA");
}

void HuaceM7xxDriverNode::CreateGmfPublisher()
{
    CreatePublisher<BESTP>(
        publisher_gmf_bestp_, gmf_bestp_period_, "BESTP");
}

void HuaceM7xxDriverNode::RequestNMEA()
{
    RequestMessage(
        MessageClass::MESSAGE_CLASS_NMEA, message::NMEA::NMEA_MESSAGE_GGA,
        message::GGA::MESSAGE_NAME, nmea_gga_period_);
}

void HuaceM7xxDriverNode::RequestGmf()
{
    RequestMessage(
        MessageClass::MESSAGE_CLASS_GMF, message::gmf::BestP::MESSAGE_ID,
        message::gmf::BestP::MESSAGE_NAME, gmf_bestp_period_);
}

void HuaceM7xxDriverNode::RegisterCallback(
    MessageClass::MESSAGE_CLASS_E message_class, uint16_t message_id,
    const std::string &message_name, const std::string &callback_name,
    message::Message::Callback callback, void *user)
{
    if (Ok != m7xx_->RegisterMessageCallback(
                  message_class, message_id, callback_name, callback, user)) {
        ROS_WARN("Failed to register %s callback.", message_name.c_str());
    }
}

void HuaceM7xxDriverNode::RegisterNMEACallback()
{
    RegisterCallback(
        MessageClass::MESSAGE_CLASS_NMEA, message::NMEA::NMEA_MESSAGE_GGA,
        message::GGA::MESSAGE_NAME, "nmea_gga",
        std::bind(&HuaceM7xxDriverNode::ProcessMessageNMEAGGA, this,
                  std::placeholders::_1, std::placeholders::_2));
}

void HuaceM7xxDriverNode::RegisterGmfCallback()
{
    RegisterCallback(
        MessageClass::MESSAGE_CLASS_GMF, message::gmf::BestP::MESSAGE_ID,
        message::gmf::BestP::MESSAGE_NAME, "gmf_bestp",
        std::bind(&HuaceM7xxDriverNode::ProcessGmfMessageBESTP, this,
                  std::placeholders::_1, std::placeholders::_2));
}

void HuaceM7xxDriverNode::HandleRtcm(
    const rtcm_msgs::Message::ConstPtr &message_data)
{
    ROS_DEBUG("Received RTCM message.");
    if (Ok != m7xx_->SendMessage(message_data->message)) {
        ROS_WARN("Failed to write RTCM message to M7xx.");
    }
}

}  // namespace huace_m7xx_ros_driver
