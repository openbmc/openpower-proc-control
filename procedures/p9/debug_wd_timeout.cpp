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
union sbeMsgReg_t
{
    uint32_t data32;
    struct
    {
        uint32_t sbeBooted : 1;
        uint32_t asyncFFDC : 1;
        uint32_t reserved1 : 2;
        uint32_t prevState : 4;
        uint32_t currState : 4;
        uint32_t majorStep : 8;
        uint32_t minorStep : 6;
        uint32_t reserved2 : 6;
    } PACKED;
};

union MboxScratch5_HB_t
{
    uint32_t data32;
    struct
    {
        uint32_t magic               :8;    //0:7
        uint32_t stepStart           :1;    //8
        uint32_t stepFinish          :1;    //9
        uint32_t reserved            :2;    //10:11
        uint32_t internalStep        :4;    //12:15
        uint32_t majorStep           :8;    //16:23
        uint32_t minorStep           :8;    //24:31
    } PACKED;
};

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
        auto readData = readReg(proc, P9_SBE_MSG_REGISTER);
        auto msg = reinterpret_cast<const sbeMsgReg_t*>(&readData);
        log<level::INFO>("SBE status register",
                         entry("PROC=%d",
                                proc->getPos()),
                         entry("SBE_MAJOR_ISTEP=%d",
                                msg->PACKED.majorStep),
                         entry("SBE_MINOR_ISTEP=%d",
                                msg->PACKED.minorStep),
                         entry("REG_VAL=%d",
                                msg->data32));

        // Read and parse HB messaging register
        readData = readReg(proc, P9_HB_MBX5_REG);
        auto hbInfo = reinterpret_cast<const MboxScratch5_HB_t*>(
                                                             &readData);
        log<level::INFO>("HB MBOX 5 register",
                         entry("PROC=%d",
                                proc->getPos()),
                         entry("HB_MAJOR_ISTEP=%d",
                                hbInfo->PACKED.majorStep),
                         entry("HB_MINOR_ISTEP=%d",
                                hbInfo->PACKED.minorStep),
                         entry("REG_VAL=%d",
                                hbInfo->data32));

    }
}

REGISTER_PROCEDURE("debugWdTimeout", debugWdTimeout);

}
}
}
