#ifndef __HUACE_MESSAGE_DRIVER_SERIAL_PORT_H__
#define __HUACE_MESSAGE_DRIVER_SERIAL_PORT_H__

#include "iointerface.h"
#include <semaphore.h>
#include <atomic>
#include <thread>
#include "../ThirdParty/BipartiteBuf/bipartite_buf.hpp"

namespace huace_m7xx_ros_driver
{
class SerialPort : public IOInterface
{
    enum ParityType {
        Odd,
        Even,
        None,
    };
    struct ConfigParam {
        bool writable;      /* default true */
        bool block_mode;    /* default false */
        uint32_t baudrate;  /* default 115200 */
        uint32_t data_bits; /* default 8 */
        uint32_t stop_bits; /* default 1 */
        ParityType parity;  /* default None */
        bool flow_control;  /* default false */
    };
public:
    SerialPort(void) = delete;
    SerialPort(const SerialPort &) = delete;
    /* name: COM1 ~ COM5, device: /dev/ttyUSB0 ... */
    SerialPort(std::string name, std::string device, uint32_t buffer_size);
    SerialPort(std::string name, std::string device, uint32_t buffer_size, ConfigParam &config);
    ~SerialPort();

    Result SetBlockMode(bool block) override final;
    Result SetBlockMode(void) override final;
    Result SetBaudRate(uint32_t baudrate) override final;
    Result SetBaudRate(void) override final;

    /* send data to M7xx */
    Result Submit(const std::string &str);
    Result Submit(const std::shared_ptr<std::string> str);
    Result Submit(const std::vector<uint8_t> &data);
    Result Submit(const std::shared_ptr<std::vector<uint8_t>> data);
    Result Submit(const uint8_t *data, uint32_t length);

    /* recv data from uart to buffer */
    void SetupSeparateThread(void) override final;
    void RequestStop(void) override final;
    bool IsStopRequested(void) const override final;
    uint32_t PrepareDataFromHardware(void);
    uint32_t PrepareDataFromBipBuffer(void);
    uint32_t PrepareDataBeforeInit(void) override final;
    uint32_t PrepareDataAfterInit(void) override final;
private:
    Result Open(bool writable);
    Result Close(void);
    Result Config(ConfigParam &config);

    int32_t fd = -1;
    std::string name;
    std::string device;
    ConfigParam config;

    sem_t read_semaphare;
    std::thread read_thread;
    std::atomic<bool> read_thread_running;
    std::atomic<bool> stop_requested;
    int32_t stop_event_fd = -1;
    static constexpr uint32_t BB_SIZE = 4 * 1024 * 1024;
    std::shared_ptr<BipartiteBuf<uint8_t, BB_SIZE>> bipbuffer;
}; /* class SerialPort */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_MESSAGE_DRIVER_SERIAL_PORT_H__ */
