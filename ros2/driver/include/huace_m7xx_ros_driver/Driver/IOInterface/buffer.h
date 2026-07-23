#ifndef __HUACE_M7XX_DRIVER_BUFFER_H__
#define __HUACE_M7XX_DRIVER_BUFFER_H__

#include <vector>
#include <memory>

namespace huace_m7xx_ros_driver
{
class Buffer
{
public:
    Buffer(void);
    Buffer(uint32_t size);
    ~Buffer();
    void Reset(void);
    bool IsEmpty(void);
    uint32_t PutData(std::vector<uint8_t> input_data);
    uint32_t PutData(std::shared_ptr<std::vector<uint8_t>> input_data);
    uint32_t PutData(std::shared_ptr<std::vector<uint8_t>> input_data, uint32_t offset, uint32_t size);
    uint32_t PutData(std::vector<uint8_t> input_data, uint32_t offset, uint32_t size);
    uint32_t GetData(std::vector<uint8_t> &output_data, uint32_t offset, uint32_t expect_size);
    uint32_t GetData(std::shared_ptr<std::vector<uint8_t>> output_data, uint32_t offset, uint32_t expect_size);

    std::shared_ptr<std::vector<uint8_t>> GetDataContainer(void);
    uint32_t UpdateReadPointer(uint32_t size);
    uint32_t UpdateWritePointer(uint32_t size);
    uint32_t GetLeftDataSize(void);
    uint32_t GetTotalSize(void);
    uint32_t Reorganization(void);
    int32_t Find(std::vector<uint8_t> pattern);

private:
    uint32_t read_pointer;
    uint32_t write_pointer;
    std::shared_ptr<std::vector<uint8_t>> data_container;

}; /* class Buffer */
} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_BUFFER_H__ */
