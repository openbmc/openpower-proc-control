#pragma once

#include <string>
#include <vector>
namespace openpower
{
namespace pel
{
using FFDCData = std::vector<std::pair<std::string, std::string>>;
namespace detail
{
/**
 * Get D-Bus service name for the specified object and interface
 *
 * @param[in] objectPath - D-Bus object path
 *
 * @param[in] interface - D-Bus interface name
 *
 * @return service name on sucess and empty string on failure
 */
std::string getService(const std::string& objectPath,
                       const std::string& interface);
} // namespace detail

/**
 * Create Boot initialization failure PEL
 *
 */
void createBootInitErrorPEL(const FFDCData& ffdcData);

/**
 * Create HWP error PEL
 *
 */
void createHWPErrorPEL(const FFDCData& ffdcData);

} // namespace pel
} // namespace openpower
