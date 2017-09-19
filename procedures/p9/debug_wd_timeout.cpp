/**
 * Copyright © 2017 IBM Corporation
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
#include <phosphor-logging/log.hpp>
#include "cfam_access.hpp"
#include "p9_cfam.hpp"
#include "registration.hpp"
#include "targeting.hpp"
#include "debug_logger.hpp"

namespace openpower
{
namespace p9
{

using namespace phosphor::logging;
using namespace openpower::cfam::access;
using namespace openpower::cfam::p9;
using namespace openpower::targeting;

namespace debug
{


/**
 * @brief Capture debug information on watchdog timeout
 * @return void
 */
void debugWdTimeout()
{
    Targeting targets;

    for(const auto& proc : targets)
    {
        // Read and parse SBE messaging register
        try
        {
            auto readData = readReg(proc, P9_SBE_MSG_REGISTER);
            auto msg = reinterpret_cast<const sbeMsgReg_t*>(&readData);
            using namespace phosphor::logging;
            log<level::INFO>("SBE status register",
                             entry("PROC=%d",
                                    proc->getPos()),
                             entry("SBE_MAJOR_ISTEP=%d",
                                    msg->PACKED.majorStep),
                             entry("SBE_MINOR_ISTEP=%d",
                                    msg->PACKED.minorStep),
                             entry("REG_VAL=%d",
                                    msg->data32));
        }
        catch (std::exception& e)
        {
            // We want to continue - capturing as much info as possible
        }
    }

    const auto& master = *(targets.begin());
    // Read and parse HB messaging register
    try
    {
        auto readData = readReg(master, P9_HB_MBX5_REG);
        auto msg = reinterpret_cast<const MboxScratch5_HB_t*>(&readData);
        using namespace phosphor::logging;
        log<level::INFO>("HB MBOX 5 register",
                         entry("HB_MAJOR_ISTEP=%d",
                                msg->PACKED.majorStep),
                         entry("HB_MINOR_ISTEP=%d",
                                msg->PACKED.minorStep),
                         entry("REG_VAL=%d",
                                msg->data32));
    }
    catch (std::exception& e)
    {
        // We want to continue - capturing as much info as possible
    }
}

REGISTER_PROCEDURE("debugWdTimeout", debugWdTimeout);

}
}
}
