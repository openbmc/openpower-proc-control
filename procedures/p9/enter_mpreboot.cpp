/**
 * Copyright (C) 2017 IBM Corporation
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
namespace p9
{

void mpipl_enter(struct pdbg_target *tgt)
{
    using namespace phosphor::logging;
    if (sbe_mpipl_enter(tgt) < 0)
    {
        auto error = errno;
        log<level::ERR>("Failed to run enter mpipl");
        throw std::system_error(error, std::generic_category(),
                                "Failed to run enter mpipl");
    }
}
/**
 * @brief Calls enter mpipl on each SBE.
 * @return void
 */
void enterMpReboot()
{
    using namespace phosphor::logging;
    struct pdbg_target* target;
    log<level::INFO>("Starting memory presrving reboot");
    std::vector<std::thread> threads;
    pdbg_targets_init(NULL);
    pdbg_for_each_class_target("pib", target)
    {
        if (pdbg_target_probe(target) != PDBG_TARGET_ENABLED)
        {
            continue;
        }
        std::thread t(mpipl_enter, target);
        threads.push_back(std::move(t));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}

REGISTER_PROCEDURE("enterMpReboot", enterMpReboot);

} // namespace p9
} // namespace openpower
