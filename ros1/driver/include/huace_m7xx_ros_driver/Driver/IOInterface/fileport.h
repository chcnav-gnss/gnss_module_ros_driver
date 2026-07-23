#ifndef __HUACE_MESSAGE_DRIVER_FILE_PORT_H__
#define __HUACE_MESSAGE_DRIVER_FILE_PORT_H__

#include "iointerface.h"

namespace huace_m7xx_ros_driver
{
class FilePort : public IOInterface
{
public:
    FilePort(void) = delete;
    FilePort(const FilePort &) = delete;
    FilePort(std::string name, std::string path, uint32_t buffer_size);
    ~FilePort();

    /* recv data from uart to buffer */
    uint32_t PrepareDataFromFile(void);
    uint32_t PrepareDataBeforeInit(void) override final;
    uint32_t PrepareDataAfterInit(void) override final;
private:
    Result Open(void);
    Result Close(void);

    int32_t fd;
    std::string name;
    std::string path;
}; /* class FilePort */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_MESSAGE_DRIVER_FILE_PORT_H__ */
