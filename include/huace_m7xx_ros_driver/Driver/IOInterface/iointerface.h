#ifndef __HUACE_M7XX_DRIVER_IO_INTERFACE_H__
#define __HUACE_M7XX_DRIVER_IO_INTERFACE_H__

#include "../huace_m7xx_common.h"
#include "buffer.h"
#include <functional>
#include <mutex>
#include <unordered_map>

namespace huace_m7xx_ros_driver
{
class IOInterface
{
public:
    using Callback = std::function<Result(const std::vector<uint8_t> &msg, void *user)>;

    IOInterface(void) = delete;
    IOInterface(uint32_t buffer_size);
    virtual ~IOInterface();

    virtual Result SetBlockMode(bool block);
    virtual Result SetBlockMode(void);
    virtual Result SetBaudRate(uint32_t baudrate);
    virtual Result SetBaudRate(void);

    /* send data to M7xx */
    virtual Result Submit(const std::string &str);
    virtual Result Submit(const std::shared_ptr<std::string> str);
    virtual Result Submit(const std::vector<uint8_t> &data);
    virtual Result Submit(const std::shared_ptr<std::vector<uint8_t>> data);
    virtual Result Submit(const uint8_t *data, uint32_t length);

    /* recv data from iointerface to buffer */
    virtual void SetupSeparateThread(void);
    virtual void RequestStop(void);
    virtual bool IsStopRequested(void) const;
    virtual uint32_t PrepareDataBeforeInit(void) = 0;
    virtual uint32_t PrepareDataAfterInit(void) = 0;

    /* recv data from buffer to output vector but do not update read addr */
    uint32_t Peek(std::vector<uint8_t> &output, uint32_t expect_size);
    uint32_t Peek(std::shared_ptr<std::vector<uint8_t>> output, uint32_t expect_size);
    uint32_t Peek(std::vector<uint8_t> &output, uint32_t offset, uint32_t expect_size);
    uint32_t Peek(std::shared_ptr<std::vector<uint8_t>> output, uint32_t offset, uint32_t expect_size);
    std::vector<uint8_t> Peek(uint32_t expect_size);
    std::vector<uint8_t> PeekLine(void);
    uint32_t Drop(uint32_t size);
    uint32_t GetBufferLeftDataSize(void);
    Result RegisterCallback(const std::string &name, Callback cb, void *user);
    Result UnregisterCallback(const std::string &name);
    bool IsCallbackRegistered(const std::string &name);
    void ExecuteCallbacks(const std::vector<uint8_t> &message);

protected:
    std::shared_ptr<Buffer> buffer;
    std::unordered_map<std::string, std::pair<Callback, void *>> callbacks;
    std::mutex callbacks_mutex;
}; /* class IOInterface */
} /* namespace huace_m7xx_ros_driver */

#endif /* __HUACE_M7XX_DRIVER_IO_INTERFACE_H__ */
