#pragma once

#include <libipl.H>

namespace openpower
{
namespace phal
{

/*
 * @Brief This function will initialize required phal
 * libraries.
 *
 * [param]iplType IPL mode, default IPL_AUTOBOOT
 */
void phal_init(enum ipl_mode mode = IPL_AUTOBOOT);

} // namespace phal
} // namespace openpower
