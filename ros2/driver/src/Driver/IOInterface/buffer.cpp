#include "huace_m7xx_ros_driver/Driver/IOInterface/buffer.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include <algorithm>

namespace huace_m7xx_ros_driver
{
    Buffer::Buffer(void)
    {
        this->read_pointer = 0;
        this->write_pointer = 0;
        this->data_container = std::make_shared<std::vector<uint8_t>>();
    }

    Buffer::Buffer(uint32_t size)
    {
        this->read_pointer = 0;
        this->write_pointer = 0;
        this->data_container = std::make_shared<std::vector<uint8_t>>();
        this->data_container->resize(size);
    }

    Buffer::~Buffer() { }

    void Buffer::Reset(void) {
        this->read_pointer = 0;
        this->write_pointer = 0;
    }

    bool Buffer::IsEmpty(void) {
        return this->read_pointer == this->write_pointer;
    }

    uint32_t Buffer::PutData(std::vector<uint8_t> input_data) {
        uint32_t write_size = std::min(input_data.size(), this->data_container->size() - this->write_pointer);
        this->data_container->insert(this->data_container->begin() + this->write_pointer,
                                     input_data.begin(),
                                     input_data.begin() + write_size);
        return write_size;
    }

    uint32_t Buffer::PutData(std::shared_ptr<std::vector<uint8_t>> input_data) {
        if (nullptr == input_data) {
            return 0;
        }
        return this->PutData(*input_data);
    }

    uint32_t Buffer::PutData(std::vector<uint8_t> input_data, uint32_t offset, uint32_t size) {
        if (offset + size > input_data.size()) {
            return 0;
        }
        uint32_t write_size = std::min(size, static_cast<uint32_t>(this->data_container->size()) - this->write_pointer);
        this->data_container->insert(this->data_container->begin() + this->write_pointer,
                                     input_data.begin() + offset,
                                     input_data.begin() + offset + write_size);
        return write_size;
    }

    uint32_t Buffer::PutData(std::shared_ptr<std::vector<uint8_t>> input_data, uint32_t offset, uint32_t size) {
        if (nullptr == input_data) {
            return 0;
        }
        return this->PutData(*input_data, offset, size);
    }
    
    uint32_t Buffer::GetData(std::vector<uint8_t> &output_data, uint32_t offset, uint32_t expect_size) {
        uint32_t left_data_size = this->write_pointer - this->read_pointer;
        if (offset + expect_size > output_data.size()) {
            output_data.resize(offset + expect_size);
        }
        uint32_t read_size = std::min(left_data_size, expect_size);
        std::copy(
            this->data_container->begin() + this->read_pointer,
            this->data_container->begin() + this->read_pointer + read_size,
            output_data.begin() + offset
        );
        return read_size;
    }
    
    uint32_t Buffer::GetData(std::shared_ptr<std::vector<uint8_t>> output_data, uint32_t offset, uint32_t expect_size) {
        if (nullptr == output_data) {
            return 0;
        }
        return this->GetData(*output_data, offset, expect_size);
    }
    
    std::shared_ptr<std::vector<uint8_t>> Buffer::GetDataContainer(void) {
        return this->data_container;
    }

    uint32_t Buffer::UpdateReadPointer(uint32_t size) {
        uint32_t update_size = std::min(size, this->write_pointer - this->read_pointer);
        this->read_pointer += update_size;
        return update_size;
    }

    uint32_t Buffer::UpdateWritePointer(uint32_t size) {
        uint32_t update_size = std::min(size, static_cast<uint32_t>(this->data_container->size()) - this->write_pointer);
        this->write_pointer += update_size;
        return update_size;
    }
    
    uint32_t Buffer::GetLeftDataSize(void) {
        return this->write_pointer - this->read_pointer;
    }

    uint32_t Buffer::GetTotalSize(void) {
        return this->data_container->size();
    }
    

    uint32_t Buffer::Reorganization(void) {
        if (this->read_pointer == 0) {
            return this->write_pointer - this->read_pointer;
        }
        uint32_t copy_size = this->write_pointer - this->read_pointer;
        std::copy(
            this->data_container->begin() + this->read_pointer,
            this->data_container->begin() + this->write_pointer,
            this->data_container->begin()
        );
        this->read_pointer = 0;
        this->write_pointer = copy_size;
        return copy_size;
    }

    int32_t Buffer::Find(std::vector<uint8_t> pattern) {
        if (this->IsEmpty()) {
            return -1;
        }
        for (auto it = this->data_container->begin() + this->read_pointer;
             it != this->data_container->begin() + this->write_pointer - pattern.size() + 1;
             ++it) {
            if (std::equal(pattern.begin(), pattern.end(), it)) {
                return std::distance(this->data_container->begin() + this->read_pointer, it);
            }
        }
        return -1;
    }
} /* namespace huace_m7xx_ros_driver */