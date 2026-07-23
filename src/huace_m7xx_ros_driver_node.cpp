#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include <exception>
#include <sys/syscall.h>

#include "huace_m7xx_ros_driver/Driver/Message/NMEA/gga.h"
#include "huace_m7xx_ros_driver/Driver/Message/GMF/bestp.h"

namespace huace_m7xx_ros_driver
{

const rclcpp::QoS HuaceM7xxDriverNode::DEFAULT_PUBLISHER_QOS = rclcpp::SystemDefaultsQoS().reliable().durability_volatile().keep_last(10);

HuaceM7xxDriverNode::HuaceM7xxDriverNode(void)
    : Node("HuaceM7xxDriverNode")
{
    this->Init();
}

HuaceM7xxDriverNode::HuaceM7xxDriverNode(const rclcpp::NodeOptions &options)
    : Node("HuaceM7xxDriverNode", options)
{
    this->Init();
}

HuaceM7xxDriverNode::~HuaceM7xxDriverNode()
{
    this->BeginShutdown();
    if (this->m7xx_thread.joinable()) {
        this->m7xx_thread.join();
    }
    this->m7xx.reset();
}

void HuaceM7xxDriverNode::BeginShutdown(void)
{
    if (this->shutdown_started.exchange(true)) {
        return;
    }

    if (nullptr != this->m7xx) {
        this->CancelAllMessages();
    }

    this->m7xx_thread_running = false;
    if (nullptr != this->m7xx) {
        this->m7xx->RequestStop();
    }
}

void HuaceM7xxDriverNode::Init(void)
{
    this->DeclareParameters();
    this->parameters_callback_handle_ = this->add_on_set_parameters_callback(std::bind(&HuaceM7xxDriverNode::parameter_callback, this, std::placeholders::_1));

    try {
        this->m7xx = std::make_shared<HuaceM7xxDriver>(std::make_shared<SerialPort>("COM1", this->com1_, 4 * 1024 * 1024), this->baudrate_);
    } catch (const std::exception &e) {
        RCLCPP_FATAL(this->get_logger(), "Failed to initialize COM1 driver for %s at baudrate %d: %s",
            this->com1_.c_str(), this->baudrate_, e.what());
        throw;
    }

    HuaceM7xxDriver::RegisterLogFnTrace([this](const std::string &msg) { RCLCPP_DEBUG(this->get_logger(), msg.c_str()); });
    HuaceM7xxDriver::RegisterLogFnDebug([this](const std::string &msg) { RCLCPP_DEBUG(this->get_logger(), msg.c_str()); });
    HuaceM7xxDriver::RegisterLogFnInfo([this](const std::string &msg) { RCLCPP_INFO(this->get_logger(), msg.c_str()); });
    HuaceM7xxDriver::RegisterLogFnWarn([this](const std::string &msg) { RCLCPP_WARN(this->get_logger(), msg.c_str()); });
    HuaceM7xxDriver::RegisterLogFnError([this](const std::string &msg) { RCLCPP_ERROR(this->get_logger(), msg.c_str()); });
    HuaceM7xxDriver::RegisterLogFnFatal([this](const std::string &msg) { RCLCPP_FATAL(this->get_logger(), msg.c_str()); });

    this->CreateServices();
    this->CancelAllMessages();

    this->CreateNMEAPublisher();
    this->RegisterNMEACallback();
    this->RequestNMEA();

    this->CreateGmfPublisher();
    this->RegisterGmfCallback();
    this->RequestGmf();

    this->subscriber_rtcm_ = this->create_subscription<rtcm_msgs::msg::Message>(
        "RTCM", DEFAULT_PUBLISHER_QOS,
        [this](rtcm_msgs::msg::Message::ConstSharedPtr msg) {
            RCLCPP_DEBUG(this->get_logger(), "Received RTCM Message.");
            if (Ok != this->m7xx->SendMessage(msg->message)) {
                RCLCPP_WARN(this->get_logger(), "Failed To Send RTCM Message into M7xx.");
            }
        });

    this->m7xx_thread_running = true;
    this->m7xx_thread = std::thread([this]() {
        pid_t tid = syscall(SYS_gettid);
        RCLCPP_INFO(this->get_logger(), "[ThreadInfo] [Huace_m7xx_driver] [Huace_m7xx_driver_node] M7xx Data Process Thread ID: %d", tid);
        while (this->m7xx_thread_running) {
            this->m7xx->ProcessMessages();
        }
    });
}

void HuaceM7xxDriverNode::CancelAllMessages(void)
{
    command::Offmsg cmd;
    this->m7xx->ExecuteCommand(cmd);
}

void HuaceM7xxDriverNode::RequestMessage(std::shared_ptr<HuaceM7xxDriver> driver, MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, const std::string msg_name, std::string period)
{
    if (!command::Outmsg::IsValidPeriod(period)) {
        return;
    }
    if (Ok != driver->RequestMessage(msg_cls, msg_id, command::Outmsg::GetOutmsgPeriodEnum(period))) {
        RCLCPP_WARN(this->get_logger(), "Request Message %s Failed.", msg_name.c_str());
    }
}

void HuaceM7xxDriverNode::CreateNMEAPublisher(void)
{
    this->CreatePublisher<nmea_msgs::msg::Sentence>(this->publisher_nmea_gga_, this->nmea_gga_period_, "NMEA/GGA");
}

void HuaceM7xxDriverNode::CreateGmfPublisher(void)
{
    this->CreatePublisher<huace_m7xx_ros_driver::msg::BESTP>(this->publisher_gmf_bestp_, this->gmf_bestp_period_, "BESTP");
}

void HuaceM7xxDriverNode::RequestNMEA(void)
{
    this->RequestMessage(this->m7xx, MessageClass::MESSAGE_CLASS_NMEA, message::NMEA::NMEA_MESSAGE_GGA, message::GGA::MESSAGE_NAME, this->nmea_gga_period_);
}

void HuaceM7xxDriverNode::RequestGmf(void)
{
    this->RequestMessage(this->m7xx, MessageClass::MESSAGE_CLASS_GMF, message::gmf::BestP::MESSAGE_ID, message::gmf::BestP::MESSAGE_NAME, this->gmf_bestp_period_);
}

void HuaceM7xxDriverNode::RegisterCallback(std::shared_ptr<HuaceM7xxDriver> driver, MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, const std::string msg_name, const std::string &cb_name, message::Message::Callback cb, void *user)
{
    if (Ok != driver->RegisterMessageCallback(msg_cls, msg_id, cb_name, cb, user)) {
        RCLCPP_WARN(this->get_logger(), "Register %s Callback Function Failed.", msg_name.c_str());
    }
}

void HuaceM7xxDriverNode::RegisterNMEACallback(void)
{
    this->RegisterCallback(this->m7xx, MessageClass::MESSAGE_CLASS_NMEA, message::NMEA::NMEA_MESSAGE_GGA, message::GGA::MESSAGE_NAME, "1", std::bind(&HuaceM7xxDriverNode::ProcessMessageNMEAGGA, this, std::placeholders::_1, std::placeholders::_2), nullptr);
}

void HuaceM7xxDriverNode::RegisterGmfCallback(void)
{
    this->RegisterCallback(this->m7xx, MessageClass::MESSAGE_CLASS_GMF, message::gmf::BestP::MESSAGE_ID, message::gmf::BestP::MESSAGE_NAME, "1", std::bind(&HuaceM7xxDriverNode::ProcessGmfMessageBESTP, this, std::placeholders::_1, std::placeholders::_2), nullptr);
}

} /* namespace huace_m7xx_ros_driver */

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(huace_m7xx_ros_driver::HuaceM7xxDriverNode)
