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
 * @param[in] interface - Interface
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
 * @brief Initiate chassis power on transition in step by step boot.
 *
 * @error InternalFailure exception thrown
 */
void chassisPowerOn();

} // namespace util
} // namespace boot
} // namespace openpower
