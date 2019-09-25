#include <sdbusplus/server.hpp>
namespace openpower
{
namespace boot
{
namespace util
{

/**
 * @brief Get the bus service
 * @param[in] bus - Bus handler
 * @param[in] path - Object Path
 * @param[in] intrtface - Interface
 *
 * @return the bus service as a string
 * @error InternalFailure exception thrown
 */
std::string getService(sdbusplus::bus::bus& bus, const std::string& path,
                       const std::string& interface);

/**
 * @brief Check whether chassis is on
 *
 * @return true if chassis is on
 * @error InternalFailure exception thrown
 */
bool isChassisOn();

/**
 * @brief Power on chassis
 *
 * @error InternalFailure exception thrown
 */
void chassisPowerOn();

/**
 * @brief Load attributes atdb file
 */
void initatdb();

/**
 * @brief Initialize targets and it will update only for master processor
 */
void initTargets();

} // namespace util
} // namespace boot
} // namespace openpower
