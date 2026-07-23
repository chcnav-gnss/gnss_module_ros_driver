#ifndef __HUACE_M7XX_DRIVER_NODE_H__
#define __HUACE_M7XX_DRIVER_NODE_H__

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <ros/ros.h>
#include <std_msgs/Header.h>
#include <nmea_msgs/Sentence.h>
#include <rtcm_msgs/Message.h>

#include <huace_m7xx_ros_driver/BESTP.h>
#include <huace_m7xx_ros_driver/OffMsg.h>
#include <huace_m7xx_ros_driver/OutMsg.h>

#include "huace_m7xx_ros_driver/Driver/huace_m7xx_ros_driver.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{

class HuaceM7xxDriverNode
{
public:
    HuaceM7xxDriverNode(ros::NodeHandle &nh, ros::NodeHandle &private_nh);
    ~HuaceM7xxDriverNode();

    void BeginShutdown();

private:
    void LoadParameters();
    void Init();
    void CreateServices();
    void CancelAllMessages();
    void RequestMessage(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id,
                        const std::string &msg_name, const std::string &period);
    void CreateNMEAPublisher();
    void CreateGmfPublisher();
    void RequestNMEA();
    void RequestGmf();
    void RegisterCallback(MessageClass::MESSAGE_CLASS_E msg_cls, uint16_t msg_id,
                          const std::string &msg_name, const std::string &callback_name,
                          message::Message::Callback callback, void *user = nullptr);
    void RegisterNMEACallback();
    void RegisterGmfCallback();

    template <typename MessageT>
    void CreatePublisher(ros::Publisher &publisher, const std::string &period,
                         const std::string &topic_name) {
        if (command::Outmsg::IsValidPeriod(period)) {
            publisher = nh_.advertise<MessageT>(topic_name, 10);
        }
    }

    Result SetPeriodParameter(const std::string &msg_name, const std::string &period,
                              int32_t *error_code);

    template <typename MessageT>
    bool SetPeriodParameter(const std::string &msg_name, const std::string &period,
                            ros::Publisher &publisher, const std::string &topic_name,
                            int32_t *error_code);

    void HandleRtcm(const rtcm_msgs::Message::ConstPtr &msg);
    bool ServiceCallbackOffMsg(OffMsg::Request &request, OffMsg::Response &response);
    bool ServiceCallbackOutMsg(OutMsg::Request &request, OutMsg::Response &response);
    void ProcessStdMsgHeader(std_msgs::Header *header);
    Result ProcessMessageNMEAGGA(const std::vector<uint8_t> &msg, void *user);
    Result ProcessGmfMessageBESTP(const std::vector<uint8_t> &msg, void *user);

    Result GetCommandResponseErrorCode(command::Command &cmd, int32_t *error_code);
    Result GetOutmsgCommandResponseErrorCode(const std::string &msg_name,
                                             command::Outmsg::OUTMSG_PERIOD_E period,
                                             int32_t *error_code);
    Result GetOffmsgCommandResponseErrorCode(const std::string &msg_name,
                                             int32_t *error_code);

    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;
    ros::Publisher publisher_nmea_gga_;
    ros::Publisher publisher_gmf_bestp_;
    ros::Subscriber subscriber_rtcm_;
    ros::ServiceServer service_off_msg_;
    ros::ServiceServer service_out_msg_;

    std::shared_ptr<HuaceM7xxDriver> m7xx_;
    std::thread m7xx_thread_;
    std::atomic<bool> m7xx_thread_running_{false};
    std::atomic<bool> shutdown_started_{false};

    std::string com1_;
    int baudrate_ = 1500000;
    std::string frame_id_;
    std::string nmea_gga_period_;
    std::string gmf_bestp_period_;

    static constexpr uint32_t TIMEOUT = 10;
    static constexpr int32_t ROS_DRIVER_ERROR_CODE = -9;
};

} /* namespace huace_m7xx_ros_driver */

#include "huace_m7xx_ros_driver_node.tpp"

#endif /* __HUACE_M7XX_DRIVER_NODE_H__ */
