#include "huace_m7xx_ros_driver/Driver/IOInterface/serialport.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include <termios.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <cerrno>
#include <stdexcept>

namespace huace_m7xx_ros_driver
{
    namespace
    {
        std::string MakeSerialPortError(const std::string &stage, const std::string &device)
        {
            return "SerialPort " + stage + " failed for device " + device;
        }
    }

    SerialPort::SerialPort(std::string name, std::string device, uint32_t buffer_size)
        : IOInterface(buffer_size), name(name), device(device), read_thread_running(false), stop_requested(false),
          bipbuffer(std::make_shared<BipartiteBuf<uint8_t, SerialPort::BB_SIZE>>()) {
        ConfigParam config;
        config.writable = true;
        config.block_mode = true;
        config.baudrate = 115200;
        config.data_bits = 8;
        config.stop_bits = 1;
        config.parity = None;
        config.flow_control = false;
        sem_init(&this->read_semaphare, 0, 0);
        this->stop_event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (this->stop_event_fd < 0) {
            Log::Warn("Failed to create SerialPort stop eventfd.");
        }

        if (Ok != this->Open(config.writable)) {
            throw std::runtime_error(MakeSerialPortError("open", this->device));
        }
        if (Ok != this->Config(config)) {
            this->Close();
            throw std::runtime_error(MakeSerialPortError("config", this->device));
        }
        Log::Trace("SerialPort init.");
    }

    SerialPort::SerialPort(std::string name, std::string device, uint32_t buffer_size, ConfigParam &config)
        : IOInterface(buffer_size), name(name), device(device), read_thread_running(false), stop_requested(false),
          bipbuffer(std::make_shared<BipartiteBuf<uint8_t, SerialPort::BB_SIZE>>()) {
        sem_init(&this->read_semaphare, 0, 0);
        this->stop_event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (this->stop_event_fd < 0) {
            Log::Warn("Failed to create SerialPort stop eventfd.");
        }
        if (Ok != this->Open(config.writable)) {
            throw std::runtime_error(MakeSerialPortError("open", this->device));
        }
        if (Ok != this->Config(config)) {
            this->Close();
            throw std::runtime_error(MakeSerialPortError("config", this->device));
        }
        Log::Trace("SerialPort init.");
    }

    SerialPort::~SerialPort() {
        Log::Trace("SerialPort Close.");
        this->RequestStop();
        if (this->read_thread.joinable()) {
            this->read_thread.join();
        }
        sem_destroy(&this->read_semaphare);
        if (this->stop_event_fd >= 0) {
            close(this->stop_event_fd);
            this->stop_event_fd = -1;
        }
        this->Close();
    }

    Result SerialPort::Open(bool writable) {
        uint32_t flags;
        if (writable) {
            flags = O_RDWR | O_NOCTTY;
        } else {
            flags = O_RDONLY | O_NOCTTY;
        }
        this->fd = open(this->device.c_str(), flags);
        if (this->fd < 0) {
            return Err;
        }
        return Ok;
    }

    Result SerialPort::SetBlockMode(bool block) {
        int32_t flags;
        if (this->fd < 0) {
            return Err;
        }
        flags = fcntl(this->fd, F_GETFL);
        if (flags < 0) {
            return Err;
        }
        if (block) {
            flags &= ~O_NONBLOCK;
        } else {
            flags |= O_NONBLOCK;
        }
        if (fcntl(this->fd, F_SETFL, flags) < 0) {
            return Err;
        }
        return Ok;
    }

    Result SerialPort::SetBlockMode(void) {
        return this->SetBlockMode(this->config.block_mode);
    }

    Result SerialPort::SetBaudRate(uint32_t baudrate) {
        struct termios opt;
        static uint32_t b_attr[] = {B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B921600, B1500000, B3000000};
        static uint32_t speed_attr[] = {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1500000, 3000000};
        bool supported = false;
        if (this->fd < 0) {
            return Err;
        }
        if (0 != tcgetattr(this->fd, &opt)) {
            return Err;
        }
        for (uint32_t index = 0; index < sizeof(speed_attr) / sizeof(uint32_t); index++) {
            if (baudrate == speed_attr[index]) {
                cfsetispeed(&opt, b_attr[index]);
                cfsetospeed(&opt, b_attr[index]);
                supported = true;
                break;
            }
        }
        if (!supported) {
            return Err;
        }
        if (0 != tcsetattr(this->fd, TCSANOW, &opt)) {
            return Err;
        }
        return Ok;
    }

    Result SerialPort::SetBaudRate(void) {
        return this->SetBaudRate(this->config.baudrate);
    }

    Result SerialPort::Config(ConfigParam &config) {
        struct termios opt;
        if (this->fd < 0) {
            return Err;
        }

        /* set block mode */
        if (Ok != this->SetBlockMode(config.block_mode)) {
            Log::Error("Set Block Mode Failed.");
            return Err;
        }

        /* set baudrate */
        if (Ok != this->SetBaudRate(config.baudrate)) {
            Log::Error("Set Serial Port Baudrate Failed.");
            return Err;
        }

        memset(&opt, 0, sizeof(opt));
        if (0 != tcgetattr(this->fd, &opt)) {
            Log::Error("Get Serial Port Attribute Failed.");
            return Err;
        }
        cfmakeraw(&opt);

        /* set data_bits, stop_bits, parity bits */
        switch (config.parity) {
        case Odd:
            opt.c_iflag |= INPCK;
            opt.c_cflag |= PARENB;
            opt.c_cflag |= PARODD;
            break;
        case Even:
            opt.c_iflag |= INPCK;
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            break;
        case None:
            opt.c_iflag &= ~INPCK;
            opt.c_cflag &= ~PARENB;
            break;
        default:
            return Err;
        }

        switch (config.data_bits) {
        case 7:
            opt.c_cflag &= ~CSIZE;
            opt.c_cflag |= CS7;
            break;
        case 8:
            opt.c_cflag &= ~CSIZE;
            opt.c_cflag |= CS8;
            break;
        default:
            return Err;
        }

        switch (config.stop_bits) {
        case 1:
            opt.c_cflag &= ~CSTOPB;
            break;
        case 2:
            opt.c_cflag |= CSTOPB;
            break;
        default:
            return Err;
        }

        /* set flow control */
        if (config.flow_control) {
            opt.c_cflag |= CRTSCTS;
        }

        /* Other settings */
        opt.c_cflag |= CLOCAL | CREAD;   /* ignore modem signal */
        opt.c_oflag &= ~OPOST;           /* do not auto convert between "\n" and "\r\n" */
        opt.c_lflag &= ~ICANON;          /* do not wait \n, read char one by one */
        opt.c_lflag &= ~(ECHO | ECHOE);  /* do not display user input in the terminal */
        opt.c_lflag &= ~ISIG;            /* Treat Ctrl+C as normal char */
        opt.c_cc[VTIME] = 0;             /* timeout (only block mode valid)*/
        opt.c_cc[VMIN] = 1;              /* at least read how many bytes (only block mode valid)*/

        if (tcsetattr(this->fd, TCSANOW, &opt) < 0) {
            return Err;
        }

        this->config = config;
        return Ok;
    }

    Result SerialPort::Close(void) {
        if (this->fd < 0) {
            return Err;
        }
        close(this->fd);
        this->fd = -1;
        return Ok;
    }

    Result SerialPort::Submit(const std::string &str) {
        int32_t actual_length;
        if (this->fd < 0) {
            return Err;
        }
        actual_length = write(this->fd, str.c_str(), str.length());
        if (actual_length < 0 || actual_length != (int32_t)str.length()) {
            return  Err;
        }
        return Ok;
    }

    Result SerialPort::Submit(const std::shared_ptr<std::string> str) {
        if (nullptr != str) {
            return this->Submit(*str);
        } else {
            return Err;
        }
    }

    Result SerialPort::Submit(const std::vector<uint8_t> &data) {
        int32_t actual_length;
        if (this->fd < 0) {
            return Err;
        }
        actual_length = write(this->fd, data.data(), data.size());
        if (actual_length < 0 || actual_length != (int32_t)data.size()) {
            return  Err;
        }
        return Ok;
    }

    Result SerialPort::Submit(const std::shared_ptr<std::vector<uint8_t>> data) {
        if (nullptr != data) {
            return this->Submit(*data);
        } else {
            return Err;
        }
    }

    Result SerialPort::Submit(const uint8_t *data, uint32_t length) {
        int32_t actual_length;
        if (this->fd < 0) {
            return Err;
        }
        actual_length = write(this->fd, data, length);
        if (actual_length < 0 || actual_length != (int32_t)length) {
            return  Err;
        }
        return Ok;
    }

    void SerialPort::SetupSeparateThread(void) {
        if (this->read_thread.joinable()) {
            throw std::runtime_error("SerialPort read thread already started for device " + this->device);
        }
        this->stop_requested = false;
        this->read_thread_running = true;
        this->read_thread = std::thread([this](){
            pid_t tid = syscall(SYS_gettid);
            Log::Info("[ThreadInfo] [Huace_m7xx_driver] [SerialPort] M7xx Data Read Thread ID: " + std::to_string(tid));

            size_t expect_len = 64;
            uint8_t *recv_buffer;
            int32_t recv_len;
            struct pollfd poll_fds[2] = {};
            nfds_t poll_fd_num = 0;

            poll_fds[poll_fd_num].fd = this->fd;
            poll_fds[poll_fd_num].events = POLLIN;
            poll_fd_num++;
            if (this->stop_event_fd >= 0) {
                poll_fds[poll_fd_num].fd = this->stop_event_fd;
                poll_fds[poll_fd_num].events = POLLIN;
                poll_fd_num++;
            }

            while(this->read_thread_running) {
                if (this->stop_requested) {
                    break;
                }
                recv_buffer = this->bipbuffer->WriteAcquire(expect_len);
                if (nullptr == recv_buffer) {
                    if (this->stop_requested) {
                        break;
                    }
                    Log::Warn("Uart BipBuffer Full. Waiting to be consumed.");
                    usleep(1000); /* sleep 1ms */
                    continue;
                }

                int32_t poll_result;
                do {
                    poll_result = poll(poll_fds, poll_fd_num, -1);
                } while (poll_result < 0 && errno == EINTR && this->read_thread_running);
                if (poll_result < 0) {
                    if (!this->read_thread_running || this->stop_requested) {
                        break;
                    }
                    Log::Fatal("Uart poll failed");
                    break;
                }
                if (poll_result == 0) {
                    continue;
                }
                if (poll_fd_num > 1 && 0 != (poll_fds[1].revents & POLLIN)) {
                    uint64_t wake_counter;
                    (void)read(this->stop_event_fd, &wake_counter, sizeof(wake_counter));
                    if (!this->read_thread_running || this->stop_requested) {
                        break;
                    }
                }
                if (0 == (poll_fds[0].revents & (POLLIN | POLLERR | POLLHUP | POLLNVAL))) {
                    continue;
                }

                recv_len = read(this->fd, recv_buffer, expect_len);
                if (recv_len < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    if (!this->read_thread_running || this->stop_requested) {
                        break;
                    }
                    Log::Fatal("Uart Read Bytes < 0");
                    break;
                }
                if (recv_len == 0) {
                    if (!this->read_thread_running || this->stop_requested) {
                        break;
                    }
                    continue;
                }
                this->bipbuffer->WriteRelease(recv_len);
                sem_post(&this->read_semaphare);

                Log::Debug("Read " + std::to_string(recv_len) + " Bytes form Uart.");
            }
        });
    }

    void SerialPort::RequestStop(void) {
        if (this->stop_requested.exchange(true)) {
            return;
        }
        this->read_thread_running = false;
        if (this->stop_event_fd >= 0) {
            uint64_t wake_counter = 1;
            (void)write(this->stop_event_fd, &wake_counter, sizeof(wake_counter));
        }
        sem_post(&this->read_semaphare);
    }

    bool SerialPort::IsStopRequested(void) const {
        return this->stop_requested;
    }

    uint32_t SerialPort::PrepareDataFromHardware(void) {
        int32_t read_bytes;
        uint32_t left_bytes;
        uint32_t desired_bytes;
        left_bytes = this->buffer->Reorganization();

        /* if file description invalid, whre will no more data */
        if (this->fd < 0) {
            return left_bytes;
        }

        /* Prepare enough space to store data */
        desired_bytes = this->buffer->GetTotalSize() - left_bytes;
        read_bytes = read(this->fd, this->buffer->GetDataContainer()->data() + left_bytes, desired_bytes);
        
        if (read_bytes < 0) {
            Log::Fatal("read_bytes < 0");
            read_bytes = 0;
        }
        this->buffer->UpdateWritePointer(read_bytes);

        /** Execute callback */
        if (read_bytes > 0) {
            std::vector<uint8_t> message = std::vector<uint8_t>(this->buffer->GetDataContainer()->begin() + left_bytes,
                                                                this->buffer->GetDataContainer()->begin() + left_bytes + read_bytes);
            this->ExecuteCallbacks(message);
        }

        /* return totall bytes num in the buffer */
        return left_bytes + read_bytes;
    }

    uint32_t SerialPort::PrepareDataBeforeInit(void) {
        return this->PrepareDataFromHardware();
    }

    uint32_t SerialPort::PrepareDataFromBipBuffer(void) {
        size_t read_bytes = 0;
        uint32_t left_bytes = this->buffer->Reorganization();

        while (sem_wait(&this->read_semaphare) < 0) {
            if (errno == EINTR) {
                if (this->stop_requested) {
                    return left_bytes;
                }
                continue;
            }
            return left_bytes;
        }

        while(true) {
            std::pair<uint8_t *, size_t> acquire_result = this->bipbuffer->ReadAcquire();
            if (nullptr == acquire_result.first) {
                /* BipBuffer Empty */
                break;
            }

            size_t available_bytes = this->buffer->GetTotalSize() - left_bytes - read_bytes;
            if (available_bytes < acquire_result.second) {
                Log::Warn("Uart Buffer Full, Please Consider Resizing The Buffer.");
            }
            size_t used_bytes = std::min(available_bytes, acquire_result.second);
            memcpy(this->buffer->GetDataContainer()->data() + left_bytes + read_bytes, acquire_result.first, used_bytes);

            this->bipbuffer->ReadRelease(used_bytes);
            this->buffer->UpdateWritePointer(used_bytes);
            read_bytes += used_bytes;
        }

        /** Execute callback */
        if (read_bytes > 0) {
            std::vector<uint8_t> message = std::vector<uint8_t>(this->buffer->GetDataContainer()->begin() + left_bytes,
                                                                this->buffer->GetDataContainer()->begin() + left_bytes + read_bytes);
            this->ExecuteCallbacks(message);
        }

        /* return totall bytes num in the buffer */
        return left_bytes + read_bytes;
    }

    uint32_t SerialPort::PrepareDataAfterInit(void) {
        return this->PrepareDataFromBipBuffer();
    }

} /* namespace huace_m7xx_ros_driver */
