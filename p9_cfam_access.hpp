#pragma once

#include <memory>
#include "targeting.hpp"

namespace openpower
{
namespace p9_cfam_access
{

constexpr uint16_t P9_FSI_A_SI1S            = 0x081C;
constexpr uint16_t P9_LL_MODE_REG           = 0x0840;
constexpr uint16_t P9_FSI2PIB_INTERRUPT     = 0x100B;
constexpr uint16_t P9_FSI2PIB_TRUE_MASK     = 0x100D;
constexpr uint16_t P9_CBS_CS                = 0x2801;
constexpr uint16_t P9_ROOT_CTRL0            = 0x2810;
constexpr uint16_t P9_PERV_CTRL0            = 0x281A;
constexpr uint16_t P9_SCRATCH_REGISTER_8    = 0x283F;
constexpr uint16_t P9_ROOT_CTRL8            = 0x2918;


/**
 * @brief Writes a CFAM (Common FRU Access Macro) register in a P9.
 *
 * Throws an exception on error.
 *
 * @param[in] - The Target to perform the operation on
 * @param[in] - The register address to write to
 * @param[in] - The data to write
 */
void writeReg(const std::shared_ptr<openpower::targeting::Target>& target,
              uint16_t address,
              uint32_t data);


/**
 * @brief Reads a CFAM (Common FRU Access Macro) register in a P9.
 *
 * Throws an exception on error.
 *
 * @param[in] - The Target to perform the operation on
 * @param[in] - The register address to read
 * @return - The register data
 */
uint32_t readReg(const std::shared_ptr<openpower::targeting::Target>& target,
                 uint16_t address);


/**
 * @brief Writes a CFAM (Common FRU Access Macro) register in a P9
 *        using a mask to specify the bits the modify.
 *
 * Only bits that are set in the mask parameter will be modified.
 *
 * Throws an exception on error.
 *
 * @param[in] - The Target to perform the operation on
 * @param[in] - The register address to write to
 * @param[in] - The data to write
 */
void writeRegWithMask(
    const std::shared_ptr<openpower::targeting::Target>& target,
    uint16_t address,
    uint32_t data,
    uint32_t mask);
}
}
