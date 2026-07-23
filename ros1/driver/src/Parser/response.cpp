#include "huace_m7xx_ros_driver/huace_m7xx_ros_driver_node.h"

#include <cerrno>
#include <functional>
#include <mutex>
#include <semaphore.h>
#include <time.h>

#include "huace_m7xx_ros_driver/Driver/Command/offmsg.h"
#include "huace_m7xx_ros_driver/Driver/Command/outmsg.h"
#include "huace_m7xx_ros_driver/Driver/Message/response.h"

namespace huace_m7xx_ros_driver
{
namespace
{

struct RegisteredCallback
{
    uint32_t class_id;
    uint16_t message_id;
    const char *name;
};

struct ResponseWaitState
{
    Result Initialize()
    {
        if (sem_init(&semaphore, 0, 0) != 0) {
            return Err;
        }
        initialized = true;
        return Ok;
    }

    ~ResponseWaitState()
    {
        if (initialized) {
            sem_destroy(&semaphore);
        }
    }

    Result Notify()
    {
        return sem_post(&semaphore) == 0 ? Ok : Err;
    }

    Result Wait(uint32_t timeout_sec,
                const std::function<bool()> &should_abort)
    {
        constexpr uint32_t wait_slice_ms = 100;
        const uint64_t max_wait_ms =
            static_cast<uint64_t>(timeout_sec) * 1000U;
        uint64_t waited_ms = 0;

        while (waited_ms < max_wait_ms) {
            if (should_abort && should_abort()) {
                return Err;
            }

            timespec deadline;
            clock_gettime(CLOCK_REALTIME, &deadline);
            deadline.tv_nsec += wait_slice_ms * 1000000L;
            deadline.tv_sec += deadline.tv_nsec / 1000000000L;
            deadline.tv_nsec %= 1000000000L;

            if (sem_timedwait(&semaphore, &deadline) == 0) {
                return Ok;
            }
            if (errno == EINTR) {
                continue;
            }
            if (errno != ETIMEDOUT) {
                return Err;
            }
            waited_ms += wait_slice_ms;
        }

        return Err;
    }

    void SetErrorCode(const std::vector<uint8_t> &message_data)
    {
        std::lock_guard<std::mutex> lock(mutex);
        error_code = static_cast<int32_t>(
            message::Response::GetErrorCode(message_data));
        has_error_code = true;
    }

    int32_t GetErrorCode(int32_t fallback) const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return has_error_code ? error_code : fallback;
    }

    sem_t semaphore{};
    bool initialized = false;
    mutable std::mutex mutex;
    int32_t error_code = 0;
    bool has_error_code = false;
};

void UnregisterCallbacks(
    const std::shared_ptr<HuaceM7xxDriver> &driver,
    const std::vector<RegisteredCallback> &callbacks)
{
    for (auto it = callbacks.rbegin(); it != callbacks.rend(); ++it) {
        if (Ok != driver->UnregisterMessageCallback(
                      it->class_id, it->message_id, it->name)) {
            ROS_WARN("Failed to unregister M7xx response callback.");
        }
    }
}

}  // namespace

Result HuaceM7xxDriverNode::GetCommandResponseErrorCode(
    command::Command &command, int32_t *error_code)
{
    if (!m7xx_ || error_code == nullptr || shutdown_started_.load()) {
        return Err;
    }

    *error_code = ROS_DRIVER_ERROR_CODE;
    std::shared_ptr<ResponseWaitState> waiter =
        std::make_shared<ResponseWaitState>();
    if (Ok != waiter->Initialize()) {
        ROS_WARN("Failed to initialize command response wait state.");
        return Err;
    }

    std::vector<RegisteredCallback> callbacks;
    const char *callback_name = "ros1_response_error_code";
    if (Ok != m7xx_->RegisterMessageCallback(
                  MessageClass::MESSAGE_CLASS_RESPONSE,
                  message::Response::MESSAGE_ID, callback_name,
                  [waiter](const std::vector<uint8_t> &message_data, void *user) {
                      (void)user;
                      waiter->SetErrorCode(message_data);
                      return waiter->Notify();
                  },
                  nullptr)) {
        ROS_WARN("Failed to register command response callback.");
        return Err;
    }
    callbacks.push_back({MessageClass::MESSAGE_CLASS_RESPONSE,
                         message::Response::MESSAGE_ID, callback_name});

    Result result = Ok;
    if (Ok != m7xx_->ExecuteCommand(command)) {
        ROS_WARN("Failed to execute %s command.",
                 command.GetCommandName().c_str());
        result = Err;
    } else if (Ok != waiter->Wait(
                          TIMEOUT,
                          [this]() { return shutdown_started_.load(); })) {
        ROS_WARN("No response received for %s command.",
                 command.GetCommandName().c_str());
        result = Err;
    }

    UnregisterCallbacks(m7xx_, callbacks);
    *error_code = waiter->GetErrorCode(*error_code);
    return result;
}

Result HuaceM7xxDriverNode::GetOutmsgCommandResponseErrorCode(
    const std::string &message_name,
    command::Outmsg::OUTMSG_PERIOD_E period, int32_t *error_code)
{
    command::Outmsg command(message_name, period);
    return GetCommandResponseErrorCode(command, error_code);
}

Result HuaceM7xxDriverNode::GetOffmsgCommandResponseErrorCode(
    const std::string &message_name, int32_t *error_code)
{
    command::Offmsg command(message_name);
    return GetCommandResponseErrorCode(command, error_code);
}

}  // namespace huace_m7xx_ros_driver
