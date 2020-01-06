#pragma once
#include <return_code.H>

#include <string>
namespace openpower
{
namespace pel
{
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
 * Create PEL by parsing the HWP return code object.
 *
 * @param[in] rc - HWP failure return code object
 *
 * @return 0 on success and -1 on failure
 */
int createHwpPel(const fapi2::ReturnCode& rc);
} // namespace pel
} // namespace openpower
