#ifndef __HUACE_M7XX_DRIVER_NODE_H__
#define __HUACE_M7XX_DRIVER_NODE_H__

#include <atomic>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"

#include "huace_m7xx_ros_driver/Driver/huace_m7xx_ros_driver.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

#include "nmea_msgs/msg/sentence.hpp"
#include "rtcm_msgs/msg/message.hpp"
#include "std_msgs/msg/header.hpp"

#include "huace_m7xx_ros_driver/msg/bestp.hpp"

#include "huace_m7xx_ros_driver/srv/off_msg.hpp"
#include "huace_m7xx_ros_driver/srv/out_msg.hpp"

namespace huace_m7xx_ros_driver
{

class HuaceM7xxDriverNode : public rclcpp::Node
{
public:
    HuaceM7xxDriverNode(void);
    HuaceM7xxDriverNode(const rclcpp::NodeOptions &options);
    ~HuaceM7xxDriverNode();

    void DeclareIntegerParameter(int32_t &param_variable, std::string param_name, int32_t default_value, std::string param_description, bool read_only);
    void DeclareStringParameter(std::string &param_variable, std::string param_name, std::string default_value, std::string param_description, bool read_only);
    void DeclarePeriodParameter(std::string &param_variable, std::string param_name, std::string default_value, std::string param_description);
    void DeclareParameters(void);
    void CreateServices(void);

    template <typename MessageT>
    void CreatePublisher(
        typename rclcpp::Publisher<MessageT>::SharedPtr &publisher,
        std::string period,
        const std::string &topic_name,
        const rclcpp::QoS &qos = DEFAULT_PUBLISHER_QOS);
    void CreateNMEAPublisher(void);
    void CreateGmfPublisher(void);

    void Init(void);
    void BeginShutdown(void);
    void CancelAllMessages(void);
    void RequestMessage(std::shared_ptr<HuaceM7xxDriver> driver, MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, const std::string msg_name, std::string period);
    void RequestNMEA(void);
    void RequestGmf(void);
    void RegisterCallback(std::shared_ptr<HuaceM7xxDriver> driver, MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id, const std::string msg_name, const std::string &cb_name, message::Message::Callback cb, void *user);
    void RegisterNMEACallback(void);
    void RegisterGmfCallback(void);
    void ProcessStdMsgHeader(std_msgs::msg::Header *header);

    Result ProcessMessageNMEAGGA(const std::vector<uint8_t> &msg, void *user);
    Result ProcessGmfMessageBESTP(const std::vector<uint8_t> &msg, void *user);

    Result GetCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, command::Command &cmd, int32_t *error_code);
    Result GetCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, std::shared_ptr<command::Command> cmd, int32_t *error_code);
    Result GetOutmsgCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, std::string msg_name, command::Outmsg::OUTMSG_PERIOD_E period, int32_t *error_code);
    Result GetOffmsgCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, std::string msg_name, int32_t *error_code);
    Result SetPeriodParameter(std::shared_ptr<HuaceM7xxDriver> driver, std::string msg_name, std::string param, rcl_interfaces::msg::SetParametersResult &result);
    template <typename MessageT>
    Result SetPeriodParameter(
        std::shared_ptr<HuaceM7xxDriver> driver,
        std::string msg_name,
        std::string param,
        rcl_interfaces::msg::SetParametersResult &result,
        typename rclcpp::Publisher<MessageT>::SharedPtr &publisher,
        const std::string &topic_name,
        const rclcpp::QoS &qos = DEFAULT_PUBLISHER_QOS);
    rcl_interfaces::msg::SetParametersResult parameter_callback(const std::vector<rclcpp::Parameter> &parameters);

    void ServiceCallbackOffMsg(const std::shared_ptr<huace_m7xx_ros_driver::srv::OffMsg::Request> request, std::shared_ptr<huace_m7xx_ros_driver::srv::OffMsg::Response> response);
    void ServiceCallbackOutMsg(const std::shared_ptr<huace_m7xx_ros_driver::srv::OutMsg::Request> request, std::shared_ptr<huace_m7xx_ros_driver::srv::OutMsg::Response> response);

private:
    rclcpp::Publisher<nmea_msgs::msg::Sentence>::SharedPtr publisher_nmea_gga_;
    rclcpp::Publisher<huace_m7xx_ros_driver::msg::BESTP>::SharedPtr publisher_gmf_bestp_;
    rclcpp::Subscription<rtcm_msgs::msg::Message>::SharedPtr subscriber_rtcm_;

    rclcpp::Service<huace_m7xx_ros_driver::srv::OffMsg>::SharedPtr service_off_msg_;
    rclcpp::Service<huace_m7xx_ros_driver::srv::OutMsg>::SharedPtr service_out_msg_;

    std::shared_ptr<HuaceM7xxDriver> m7xx;
    std::thread m7xx_thread;
    std::atomic<bool> m7xx_thread_running{false};
    std::atomic<bool> shutdown_started{false};

    std::string com1_;
    int32_t baudrate_;
    std::string frame_id_;
    std::string nmea_gga_period_;
    std::string gmf_bestp_period_;

    OnSetParametersCallbackHandle::SharedPtr parameters_callback_handle_;

    static constexpr uint32_t TIMEOUT = 10;
    static constexpr int32_t ROS_DRIVER_ERROR_CODE = -9;
    static const std::string DEFAULT_PARAM_STRING;
    static const std::string DEFAULT_MESSAGE_PERIOD;
    static const rclcpp::QoS DEFAULT_PUBLISHER_QOS;
};

} /* namespace huace_m7xx_ros_driver */

#include "huace_m7xx_ros_driver_node.tpp"

#endif /* __HUACE_M7XX_DRIVER_NODE_H__ */
