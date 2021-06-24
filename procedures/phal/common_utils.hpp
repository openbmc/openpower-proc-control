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
 *  @param[in] procTarget - Target to check if master or not
 *
 *  @return True/False
 */
bool isMasterProc(struct pdbg_target* procTarget);

/**
 *  @brief  Read the input CFAM register
 *
 *  @param[in]  procTarget - The Target to perform the operation on
 *  @param[in]  reg - The register address to read
 *  @param[out] val - The value read from the register
 *
 *  @return 0 on success, non-0 on failure
 */
uint32_t getCFAM(struct pdbg_target* procTarget, const uint16_t reg,
                 uint32_t& val);

/**
 *  @brief  Write the input CFAM register
 *
 *  @param[in]  procTarget - The Target to perform the operation on
 *  @param[in]  reg - The register address to write
 *  @param[out] val - The value to write to the register
 *
 *  @return 0 on success, non-0 on failure
 */
uint32_t putCFAM(struct pdbg_target* procTarget, const uint16_t reg,
                 const uint32_t val);

/**
 *  @brief  Helper function to find FSI target needed for FSI operations
 *
 *  @param[in]  procTarget - The processor Target to find the FSI target on
 *
 *  @return Valid pointer to FSI target on success, nullptr on failure
 */
pdbg_target* getFsiTarget(struct pdbg_target* procTarget);

} // namespace phal
} // namespace openpower
