// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace openpower
{
namespace phal
{
namespace env
{

/**
 * @brief Helper function to set PDBG_DTB
 *
 * PDBG_DTB environment variable set to CEC device tree path
 *
 * @return void on success.
 *         Throws an exception on failure.
 */
void setDevtreeEnv();

/**
 * @brief Helper function to set PDATA_INFODB
 *
 * PDATA_INFODB environment variable set to pdata attributes infodb path
 *
 * @return void on success.
 *         Throws an exception on failure.
 */
void setPdataInfoDBEnv();

/**
 * @brief Used to get the log level from the given environment variable
 *
 * @param[in] env - The name of the environment variable
 * @param[in] dValue - The default log level
 *
 * @return The log level of the given environment variable
 *
 * @note This function will return the given default log level if failed
 *       to find the given environment variable log level
 */
uint8_t getLogLevelFromEnv(const char* env, const uint8_t dValue);

} // namespace env
} // namespace phal
} // namespace openpower
