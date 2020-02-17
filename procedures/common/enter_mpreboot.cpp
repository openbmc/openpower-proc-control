/**
 * Copyright (C) 2020 IBM Corporation
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

#include <libpdbg.h>

#include <phosphor-logging/log.hpp>
#include <thread>
#include <vector>

namespace openpower
{
namespace misc
{

/**
 * @brief Calls sbe_enter_mpipl on the SBE in the provided target.
 * @return void
 */
void sbeEnterMpReboot(struct pdbg_target* tgt)
{
    using namespace phosphor::logging;
    if (sbe_mpipl_enter(tgt) < 0)
    {
        auto error = errno;
        log<level::ERR>("Failed to initiate memory preserving reboot");
        // TODO Create a PEL in the future for this failure case.
        throw std::system_error(error, std::generic_category(),
                                "Failed to initiate memory preserving reboot");
    }
}
/**
 * @brief initiate memory preserving reboot on each SBE.
 * @return void
 */
void enterMpReboot()
{
    using namespace phosphor::logging;
    struct pdbg_target* target;
    std::vector<std::thread> threads;
    pdbg_targets_init(NULL);

    log<level::INFO>("Starting memory preserving reboot");
    pdbg_for_each_class_target("pib", target)
    {
        if (pdbg_target_probe(target) != PDBG_TARGET_ENABLED)
        {
            continue;
        }
        std::thread t(sbeEnterMpReboot, target);
        threads.push_back(std::move(t));
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

REGISTER_PROCEDURE("enterMpReboot", enterMpReboot);

} // namespace misc
} // namespace openpower
