#pragma once

#include <libipl.H>

extern "C"
{
#include <libpdbg.h>
}

namespace openpower
{
namespace phal
{

/**
 * @brief This function will initialize required phal
 *        libraries.
 * Throws an exception on error.
 *
 * @param[in] mode - IPL mode, default IPL_AUTOBOOT
 *
 */
void phal_init(enum ipl_mode mode = IPL_AUTOBOOT);

/**
 *  @brief  Check if master processor or not
 *
 *  * @param[in] procTarget - Target to check if master or not
 *
 *  @return True/False
 */
bool isMasterProc(struct pdbg_target* procTarget);

} // namespace phal
} // namespace openpower
