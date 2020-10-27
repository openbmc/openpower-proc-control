/**
 * Copyright (C) 2018 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cfam_access.hpp"
#include "p9_cfam.hpp"
#include "registration.hpp"
#include "targeting.hpp"

#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

namespace openpower
{
namespace p9
{

using namespace phosphor::logging;
using namespace openpower::cfam::access;
using namespace openpower::cfam::p9;
using namespace openpower::targeting;
namespace file_error = sdbusplus::xyz::openbmc_project::Common::File::Error;

/**
 * @brief Disables PCIE drivers and receiver in the PCIE root ctrl 1 register
 * @return void
 */
void cleanupPcie()
{
    try
    {
        Targeting targets;

        log<level::INFO>("Running P9 procedure cleanupPcie");

        // Disable the PCIE drivers and receiver on all CPUs
        for (const auto& target : targets)
        {
            try
            {
                writeReg(target, P9_ROOT_CTRL1_CLEAR, 0x00001C00);
            }
            catch (std::exception& e)
            {
                // Don't need an error log coming from the power off
                // path, just keep trying on the other processors.
                continue;
            }
        }
    }
    catch (file_error::Open& e)
    {
        // For this procedure we can ignore the ::Open error
    }
}

REGISTER_PROCEDURE("cleanupPcie", cleanupPcie)

} // namespace p9
} // namespace openpower
