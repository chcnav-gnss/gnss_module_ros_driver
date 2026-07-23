#include "huace_m7xx_ros_driver/Driver/IOInterface/fileport.h"
#include "huace_m7xx_ros_driver/Driver/huace_m7xx_log.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>
#include <unistd.h>

namespace huace_m7xx_ros_driver
{
    FilePort::FilePort(std::string name, std::string path, uint32_t buffer_size)
        : IOInterface(buffer_size), name(name), path(path)
    {
        if (Ok != this->Open()) {
            Log::Trace("FilePort Open Failed");
            return;
        }
        Log::Trace("FilePort init.");
    }

    FilePort::~FilePort()
    {
        this->Close();
        Log::Trace("FilePort Close.");
    }

    Result FilePort::Open() {
        this->fd = open(this->path.c_str(), O_RDONLY);
        if (this->fd < 0) {
            return Err;
        }
        return Ok;
    }

    Result FilePort::Close(void) {
        if (this->fd < 0) {
            return Err;
        }
        close(this->fd);
        this->fd = -1;
        return Ok;
    }

    uint32_t FilePort::PrepareDataFromFile(void) {
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

    uint32_t FilePort::PrepareDataBeforeInit(void) {
        return this->PrepareDataFromFile();
    }

    uint32_t FilePort::PrepareDataAfterInit(void) {
        return this->PrepareDataFromFile();
    }

} /* namespace huace_m7xx_ros_driver */
