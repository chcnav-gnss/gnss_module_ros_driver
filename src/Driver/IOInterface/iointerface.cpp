#include "huace_m7xx_ros_driver/Driver/IOInterface/iointerface.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"

namespace huace_m7xx_ros_driver
{
    IOInterface::IOInterface(uint32_t buffer_size)
    {
        this->buffer = std::make_shared<Buffer>(buffer_size);
    }

    IOInterface::~IOInterface() { }

    Result IOInterface::SetBlockMode(bool block) {
        (void) block;
        return Ok;
    }

    Result IOInterface::SetBlockMode(void) {
        return Ok;
    }

    Result IOInterface::SetBaudRate(uint32_t baudrate) {
        (void) baudrate;
        return Ok;
    }

    Result IOInterface::SetBaudRate(void) {
        return Ok;
    }

    Result IOInterface::Submit(const std::string &str) {
        (void) str;
        return Ok;
    }

    Result IOInterface::Submit(const std::shared_ptr<std::string> str) {
        (void) str;
        return Ok;
    }

    Result IOInterface::Submit(const std::vector<uint8_t> &data) {
        (void) data;
        return Ok;
    }

    Result IOInterface::Submit(const std::shared_ptr<std::vector<uint8_t>> data) {
        (void) data;
        return Ok;
    }

    Result IOInterface::Submit(const uint8_t *data, uint32_t length) {
        (void) data;
        (void) length;
        return Ok;
    }

    void IOInterface::SetupSeparateThread(void) {
        return;
    }

    void IOInterface::RequestStop(void) {
        return;
    }

    bool IOInterface::IsStopRequested(void) const {
        return false;
    }

    uint32_t IOInterface::Peek(std::vector<uint8_t> &output, uint32_t expect_size) {
        if (expect_size > static_cast<uint32_t>(output.size())) {
            output.resize(expect_size);
        }
        /* return how many byte had been put into output vector just now */
        return this->buffer->GetData(output, 0, expect_size);
    }

    uint32_t IOInterface::Peek(std::shared_ptr<std::vector<uint8_t>> output, uint32_t expect_size) {
        if (expect_size > static_cast<uint32_t>(output->size())) {
            output->resize(expect_size);
        }
        /* return how many byte had been put into output vector just now */
        return this->buffer->GetData(output, 0, expect_size);
    }

    uint32_t IOInterface::Peek(std::vector<uint8_t> &output, uint32_t offset, uint32_t expect_size) {
        if (offset + expect_size > static_cast<uint32_t>(output.size())) {
            output.resize(offset + expect_size);
        }
        /* return how many byte had been put into output vector just now */
        return this->buffer->GetData(output, offset, expect_size);
    }

    uint32_t IOInterface::Peek(std::shared_ptr<std::vector<uint8_t>> output, uint32_t offset, uint32_t expect_size) {
        if (offset + expect_size > static_cast<uint32_t>(output->size())) {
            output->resize(offset + expect_size);
        }
        /* return how many byte had been put into output vector just now */
        return this->buffer->GetData(output, offset, expect_size);
    }

    std::vector<uint8_t> IOInterface::Peek(uint32_t expect_size) {
        std::vector<uint8_t> vec = std::vector<uint8_t>(expect_size, 0);

        uint32_t actual_size = this->buffer->GetData(vec, 0, expect_size);

        /* if there is not enough data in the buffer, return a vector with not enough data */
        /* if no data in the buffer, return a empty vector */
        if (expect_size != actual_size) {
            vec.resize(actual_size);
        }
        return vec;
    }

    std::vector<uint8_t> IOInterface::PeekLine(void) {
        std::vector<uint8_t> pattern = {'\r', '\n'};
        int32_t cr_index = this->buffer->Find(pattern);
        if (cr_index < 0) {
            return std::vector<uint8_t>();
        }
        std::vector<uint8_t> vec = std::vector<uint8_t>(cr_index + 2, 0);  /**< len("\r\n") = 2 */
        this->buffer->GetData(vec, 0, cr_index + 2);
        /* if there is not enough data in the buffer, return a vector with not enough data */
        /* if no data in the buffer, return a empty vector */
        return vec;
    }

    uint32_t IOInterface::Drop(uint32_t size) {
        /* return how many byte had been droped */
        return this->buffer->UpdateReadPointer(size);
    }

    uint32_t IOInterface::GetBufferLeftDataSize(void) {
        return this->buffer->GetLeftDataSize();
    }

    Result IOInterface::RegisterCallback(const std::string &name, Callback cb, void *user) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        if (0 != this->callbacks.count(name)) {
            return Err;
        }
        this->callbacks.insert({name, std::pair(cb, user)});
        return Ok;
    }

    Result IOInterface::UnregisterCallback(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        if (0 == this->callbacks.count(name)) {
            return Err;
        }
        this->callbacks.erase(name);
        return Ok;
    }

    bool IOInterface::IsCallbackRegistered(const std::string &name) {
        std::lock_guard<std::mutex> lock(this->callbacks_mutex);
        return 0 != this->callbacks.count(name);
    }

    void IOInterface::ExecuteCallbacks(const std::vector<uint8_t> &message) {
        std::unordered_map<std::string, std::pair<Callback, void *>> callbacks_snapshot;
        {
            std::lock_guard<std::mutex> lock(this->callbacks_mutex);
            callbacks_snapshot = this->callbacks;
        }

        for (auto it = callbacks_snapshot.begin(); it != callbacks_snapshot.end(); ++it) {
            if (Ok != it->second.first(message, it->second.second)) {
                Log::Warn("Callback Function " + it->first + " Return Err.");
            }
        }
    }

} /* namespace huace_m7xx_ros_driver */
