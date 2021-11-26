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
 *
 * @param[in] iplType - the IPL type
 * @param[in] iplErrCallback - the callback to handle IPL error
 *
 * @return NULL on success
 *         Throws an exception on failures
 *
 * @note Refer https://github.com/open-power/ipl/blob/main/libipl/libipl.H
 *       to get more details about the supported ipl mode and type
 */
void phal_init(const enum ipl_type& iplType,
               const ipl_error_callback_func_t iplErrCallback);

/**
 *  @brief  Check if primary processor or not
 *
 *  @param[in] procTarget - Processor target to check if primary or not
 *
 *  @return True/False
 */
bool isPrimaryProc(struct pdbg_target* procTarget);

} // namespace phal
} // namespace openpower
