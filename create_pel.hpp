#pragma once
#include <return_code.H>

#include <string>
namespace openpower
{
namespace pel
{
namespace detail
{
std::string getService(const std::string& objectPath,
                       const std::string& interface);
} // namespace detail

/**
 * Create PEL by parsing the return code object returned
 * by the Hardware Procedure.
 */
void createHwpPel(const fapi2::ReturnCode& rc);
} // namespace pel
} // namespace openpower
