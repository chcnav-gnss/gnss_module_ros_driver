#ifndef __HUACE_M7XX_DRIVER_LOG_H__
#define __HUACE_M7XX_DRIVER_LOG_H__

#include <functional>
#include <string>

namespace huace_m7xx_ros_driver
{
class Log
{
public:
    using LogFn = std::function<void(const std::string &msg)>;

    typedef struct _LOG_FUNC_GROUP_T {
        LogFn Trace;
        LogFn Debug;
        LogFn Info;
        LogFn Warn;
        LogFn Error;
        LogFn Fatal;
    } LOG_FUNC_GROUP_T;

    static void Trace(const std::string &msg);
    static void Debug(const std::string &msg);
    static void Info(const std::string &msg);
    static void Warn(const std::string &msg);
    static void Error(const std::string &msg);
    static void Fatal(const std::string &msg);
    static LOG_FUNC_GROUP_T logger;
}; /* class Log */

} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_LOG_H__ */