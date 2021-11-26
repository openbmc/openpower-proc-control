// SPDX-License-Identifier: Apache-2.0

#pragma once

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

} // namespace env
} // namespace phal
} // namespace openpower
