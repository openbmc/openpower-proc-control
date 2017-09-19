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

namespace sbe
{
typedef struct
{
    uint32_t sbeBooted : 1;
    uint32_t asyncFFDC : 1;
    uint32_t reserved1 : 2;
    uint32_t prevState : 4;
    uint32_t currState : 4;
    uint32_t majorStep : 8;
    uint32_t minorStep : 6;
    uint32_t reserved2 : 6;
} sbeMsgReg_t;

std::map<uint8_t, std::string> sbeStates =
{ {0x00, "SBE_STATE_UNKNOWN"},
  {0x01, "SBE_STATE_IPLING"},
  {0x02, "SBE_STATE_ISTEP"},
  {0x03, "SBE_STATE_MPIPL"},
  {0x04, "SBE_STATE_RUNTIME"},
  {0x05, "SBE_STATE_DMT"},
  {0x06, "SBE_STATE_DUMP"},
  {0x07, "SBE_STATE_FAILURE"},
  {0x08, "SBE_STATE_QUIESCE"},
  {0x0F, "SBE_INVALID_STATE"},
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
    log<level::INFO>("Running P9 SBE debug");

    Targeting targets;

    for(const auto& proc : targets)
    {
        // Read and parse SBE messaging register
        cfam_data_t readData = readReg(proc, P9_SBE_MSG_REGISTER);
        const sbeMsgReg_t* msg = reinterpret_cast<const sbeMsgReg_t*>(
                                                             &readData);

        auto prevState = sbeStates.find(msg->prevState);
        auto currState = sbeStates.find(msg->currState);
        std::stringstream prevStateStr, currStateStr;
        if(prevState != sbeStates.end() && currState != sbeStates.end())
        {
            prevStateStr << prevState->second;
            currStateStr << currState->second;
        }
        else
        {
            prevStateStr << msg->prevState;
            currStateStr << msg->currState;
        }
        log<level::INFO>("SBE status register:",
                         entry("PROC:%d\n"
                               "SBE_BOOTED=%s\n"
                               "ASYNC_FFDC=%s\n"
                               "SBE_PREV_STATE=%s\n"
                               "SBE_CURR_STATE=%s\n"
                               "SBE_MAJOR_ISTEP=%d\n"
                               "SBE_MINOR_ISTEP=%d",
                                proc->getPos(),
                                msg->sbeBooted ? "True":"Flase",
                                msg->asyncFFDC ? "True":"Flase",
                                prevStateStr.str().c_str(),
                                currStateStr.str().c_str(),
                                msg->majorStep,
                                msg->minorStep));

        // Read and parse HB messaging register
        readData = readReg(proc, P9_HB_MBX5_REG);
        const MboxScratch5_HB_t* hbInfo = reinterpret_cast<const MboxScratch5_HB_t*>(
                                                             &readData);

        if(hbInfo->PACKED.magic == 0xAA)
        {
            log<level::INFO>("HB MBOX 5 reg:",
                             entry("PROC:%d\n"
                                   "HB_STEP_START=%d\n"
                                   "HB_STEP_FINISH=%d\n"
                                   "HB_INTERNAL_STEP=%s\n"
                                   "HB_MAJOR_ISTEP=%d\n"
                                   "HB_MINOR_ISTEP=%d",
                                    proc->getPos(),
                                    hbInfo->PACKED.stepStart,
                                    hbInfo->PACKED.stepFinish,
                                    hbInfo->PACKED.internalStep,
                                    hbInfo->PACKED.majorStep,
                                    hbInfo->PACKED.minorStep));
        }
        else
        {
            log<level::INFO>("HB MBOX 5 reg:",
                             entry("PROC:%d\n"
                                   "Value=0x%08x",
                                    proc->getPos(),
                                    hbInfo->data32));
        }
    }
}

REGISTER_PROCEDURE("debugWdTimeout", debugWdTimeout);

}
}
}
