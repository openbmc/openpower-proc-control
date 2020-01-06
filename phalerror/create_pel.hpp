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
 * Create Boot initialization failure PEL
 *
 */
void createBootInitErrorPEL();

/**
 * Create PEL by parsing the FAPI ReturnCode object
 *
 * @param[in] rc - FAPI ReturnCode object
 *
 */
void createFAPIReturnCodeErrorPEL(const fapi2::ReturnCode& rc);
} // namespace pel
} // namespace openpower
