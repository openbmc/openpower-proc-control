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

/**
 * @brief SBE debug information
 * @return void
 */
void sbeDebug()
{
    log<level::INFO>("Running P9 SBE debug");

    Targeting targets;

    for(auto& proc : targets)
    {
        // Read and parse SBE messaging register
        const cfam_data_t readData = readReg(proc, P9_SBE_MSG_REGISTER);
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
        // Log an errorlog entry
        elog<org::open_power::Proc::SBE::Status>(
         org::open_power::Proc::SBE::Status::PROC(proc->getPos()),
         org::open_power::Proc::SBE::Status::SBE_BOOTED(
                                 msg->sbeBooted ? "True":"Flase"),
         org::open_power::Proc::SBE::Status::ASYNC_FFDC(
                                 msg->asyncFFDC ? "True":"Flase"),
         org::open_power::Proc::SBE::Status::SBE_PREV_STATE(
                                      prevStateStr.str().c_str()),
         org::open_power::Proc::SBE::Status::SBE_CURR_STATE(
                                      currStateStr.str().c_str()),
         org::open_power::Proc::SBE::Status::SBE_MAJOR_ISTEP(msg->majorStep),
         org::open_power::Proc::SBE::Status::SBE_MINOR_ISTEP(msg->minorStep));
    }

}

REGISTER_PROCEDURE("sbeDebug", sbeDebug);

}
}
}
