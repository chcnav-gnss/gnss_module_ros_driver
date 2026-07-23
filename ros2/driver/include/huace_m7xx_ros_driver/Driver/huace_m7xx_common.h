#ifndef __HUACE_M7XX_DRIVER_COMMON_H__
#define __HUACE_M7XX_DRIVER_COMMON_H__
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace huace_m7xx_ros_driver {

    enum Result {
        Ok           = 0,
        Err          = -1,
    };

    uint8_t CalculateXOR(const std::string &str, uint32_t offset, uint32_t len);
    uint8_t CalculateXOR(const std::shared_ptr<std::string> str, uint32_t offset, uint32_t len);
    std::string CalculateXOR_S(const std::string &str, uint32_t offset, uint32_t len);
    std::string CalculateXOR_S(const std::shared_ptr<std::string> str, uint32_t offset, uint32_t len);

    uint8_t CalculateXOR(const std::vector<uint8_t> &vec, uint32_t offset, uint32_t len);
    uint8_t CalculateXOR(const std::shared_ptr<std::vector<uint8_t>> vec, uint32_t offset, uint32_t len);
    std::vector<uint8_t> CalculateXOR_S(const std::vector<uint8_t> &vec, uint32_t offset, uint32_t len);
    std::vector<uint8_t> CalculateXOR_S(const std::shared_ptr<std::vector<uint8_t>> vec, uint32_t offset, uint32_t len);

    uint32_t CalculateCRC32(const std::vector<uint8_t> &vec, uint32_t offset, uint32_t len);
    uint32_t CalculateCRC32(const std::shared_ptr<std::vector<uint8_t>> vec, uint32_t offset, uint32_t len);
    uint32_t CalculateCRC32(uint8_t *buffer, uint32_t offset, uint32_t len);

} /* namespace huace_m7xx_ros_driver */
#endif /* __HUACE_M7XX_DRIVER_COMMON_H__ */
