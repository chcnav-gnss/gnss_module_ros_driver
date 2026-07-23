#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"
#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/Command/outmsg.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"
#include <errno.h>
#include <functional>
#include <mutex>
#include <semaphore.h>
#include <time.h>

namespace huace_m7xx_ros_driver {
namespace {

struct RegisteredCallback {
    uint32_t class_id;
    uint16_t message_id;
    const char *name;
};

struct ResponseWaitState {
    explicit ResponseWaitState(uint32_t expected_signal_count)
        : expected_signal_count(expected_signal_count) {
    }

    ~ResponseWaitState() {
        if (sem_initialized) {
            sem_destroy(&sem);
        }
    }

    Result Init() {
        if (0 != sem_init(&sem, 0, 0)) {
            return Err;
        }
        sem_initialized = true;
        return Ok;
    }

    Result Notify() {
        return (0 == sem_post(&sem)) ? Ok : Err;
    }

    Result Wait(uint32_t timeout_sec, const std::function<bool(void)> &should_abort) {
        constexpr uint32_t wait_slice_ms = 100;
        const uint64_t max_wait_ms = static_cast<uint64_t>(timeout_sec) * 1000U;
        uint64_t waited_ms = 0;

        for (uint32_t i = 0; i < expected_signal_count; ++i) {
            while (waited_ms < max_wait_ms) {
                if (should_abort && should_abort()) {
                    return Err;
                }

                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_nsec += wait_slice_ms * 1000000;
                ts.tv_sec += ts.tv_nsec / 1000000000;
                ts.tv_nsec %= 1000000000;

                if (0 == sem_timedwait(&sem, &ts)) {
                    break;
                }
                if (EINTR == errno) {
                    continue;
                }
                if (ETIMEDOUT != errno) {
                    return Err;
                }
                waited_ms += wait_slice_ms;
            }

            if (waited_ms >= max_wait_ms) {
                return Err;
            }
        }
        return Ok;
    }

    void SetErrorCode(const std::vector<uint8_t> &msg) {
        std::lock_guard<std::mutex> lock(mutex);
        error_code = static_cast<int32_t>(message::Response::GetErrorCode(msg));
        has_error_code = true;
    }

    void CopyErrorCodeTo(int32_t *out_error_code) const {
        if (nullptr == out_error_code) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex);
        if (has_error_code) {
            *out_error_code = error_code;
        }
    }

    sem_t sem{};
    bool sem_initialized = false;
    uint32_t expected_signal_count = 0;

    mutable std::mutex mutex;
    int32_t error_code = 0;
    bool has_error_code = false;
};

Result RegisterTrackedCallback(
        std::shared_ptr<HuaceM7xxDriver> driver,
        std::vector<RegisteredCallback> &registered_callbacks,
        uint32_t class_id,
        uint16_t message_id,
        const char *callback_name,
        message::Message::Callback callback) {
    if (Ok != driver->RegisterMessageCallback(class_id, message_id, callback_name, callback, nullptr)) {
        return Err;
    }

    registered_callbacks.push_back({class_id, message_id, callback_name});
    return Ok;
}

void UnregisterTrackedCallbacks(
        std::shared_ptr<HuaceM7xxDriver> driver,
        const std::vector<RegisteredCallback> &registered_callbacks,
        rclcpp::Logger logger) {
    for (auto it = registered_callbacks.rbegin(); it != registered_callbacks.rend(); ++it) {
        if (Ok != driver->UnregisterMessageCallback(it->class_id, it->message_id, it->name)) {
            RCLCPP_WARN(logger, "UnRegisterMessageCallback Failed.");
        }
    }
}

}  // namespace

Result HuaceM7xxDriverNode::GetCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, command::Command &cmd, int32_t *error_code) {
    if (nullptr == driver || nullptr == error_code) {
        return Err;
    }

    *error_code = ROS_DRIVER_ERROR_CODE;
    if (this->shutdown_started.load()) {
        RCLCPP_WARN(this->get_logger(), "Skip command %s while shutting down.", cmd.GetCommandName().c_str());
        return Err;
    }

    auto waiter = std::make_shared<ResponseWaitState>(1);
    if (Ok != waiter->Init()) {
        RCLCPP_WARN(this->get_logger(), "Initialize wait state failed.");
        return Err;
    }

    waiter->error_code = *error_code;
    waiter->has_error_code = true;

    std::vector<RegisteredCallback> registered_callbacks;
    if (Ok != RegisterTrackedCallback(driver, registered_callbacks, MessageClass::MESSAGE_CLASS_RESPONSE, message::Response::MESSAGE_ID, "response_error_code",
            [waiter](const std::vector<uint8_t> &msg, void *user) -> Result {
                (void)user;
                waiter->SetErrorCode(msg);
                return waiter->Notify();
            })) {
        RCLCPP_WARN(this->get_logger(), "RegisterMessageCallback Failed.");
        return Err;
    }

    Result res = Ok;
    if (this->shutdown_started.load()) {
        res = Err;
    } else if (Ok != driver->ExecuteCommand(cmd)) {
        RCLCPP_WARN(this->get_logger(), "Execute Command %s Failed.", cmd.GetCommandName().c_str());
        res = Err;
    } else if (Ok != waiter->Wait(TIMEOUT, [this]() { return this->shutdown_started.load(); })) {
        if (this->shutdown_started.load()) {
            RCLCPP_WARN(this->get_logger(), "Command %s interrupted by shutdown.", cmd.GetCommandName().c_str());
        } else {
            RCLCPP_WARN(this->get_logger(), "No reply message received from M7xx.");
        }
        res = Err;
    }

    UnregisterTrackedCallbacks(driver, registered_callbacks, this->get_logger());
    waiter->CopyErrorCodeTo(error_code);

    return res;
}

Result HuaceM7xxDriverNode::GetCommandResponseErrorCode(std::shared_ptr<HuaceM7xxDriver> driver, std::shared_ptr<command::Command> cmd, int32_t *error_code) {
    if (nullptr == driver || nullptr == cmd || nullptr == error_code) {
        return Err;
    }
    return this->GetCommandResponseErrorCode(driver, *cmd, error_code);
}

}  // namespace huace_m7xx_ros_driver
