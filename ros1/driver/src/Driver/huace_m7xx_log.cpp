#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
    Log::LOG_FUNC_GROUP_T Log::logger = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    void Log::Trace(const std::string &msg) {
        if (nullptr != Log::logger.Trace) Log::logger.Trace(msg);
    }
    void Log::Debug(const std::string &msg) {
        if (nullptr != Log::logger.Debug) Log::logger.Debug(msg);
    }
    void Log::Info(const std::string &msg) {
        if (nullptr != Log::logger.Info) Log::logger.Info(msg);
    }
    void Log::Warn(const std::string &msg) {
        if (nullptr != Log::logger.Warn) Log::logger.Warn(msg);
    }
    void Log::Error(const std::string &msg) {
        if (nullptr != Log::logger.Error) Log::logger.Error(msg);
    }
    void Log::Fatal(const std::string &msg) {
        if (nullptr != Log::logger.Fatal) Log::logger.Fatal(msg);
    }
} /* namespace huace_m7xx_ros_driver */
