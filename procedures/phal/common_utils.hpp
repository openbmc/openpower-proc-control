#pragma once

#include <libipl.H>

namespace openpower
{
namespace phal
{

/**
 * @brief This function will initialize required phal
 *        libraries.
 *
 * @param[in] iplErrCallbackFunc used to pass error callback to handle
 *            ipl failures, default NULL
 * @param[in] iplMode used to pass ipl mode, default IPL_AUTOBOOT
 * @param[in] iplType used to pass ipl type, default IPL_TYPE_NORMAL
 *
 * @return NULL on success
 *         Throws an exception on error.
 */
void initPHAL(const ipl_error_callback_func_t iplErrCallbackFunc = nullptr,
              const enum ipl_mode iplMode = IPL_AUTOBOOT,
              const enum ipl_type iplType = IPL_TYPE_NORMAL);

} // namespace phal
} // namespace openpower
