#include <sdbusplus/server.hpp>
namespace open_power
{
namespace boot
{
namespace util
{

/**
 * @brief Get the bus service
 *
 * @return the bus service as a string
 */
std::string getService(sdbusplus::bus::bus& bus, const std::string& path,
                       const std::string& interface);

/**
 * @brief Check whether chassis is on
 *
 * @return true if chassis is ong
 */
bool isChassisOn();

/**
 * @brief Power on chassis
 *
 * @return 0 for success
 */
int chassisPowerOn();

} //namespace util
} //namespace boot
} //namespace open_power
