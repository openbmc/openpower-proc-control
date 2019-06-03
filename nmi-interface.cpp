/**
 * Copyright (C) 2019 IBM Corporation
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

#include "nmi-interface.hpp"
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Common/error.hpp>
#include "targeting.hpp"

#if 0
#include <sbe_interfaces.hpp>
#endif

namespace openpower
{
namespace proc
{

using namespace phosphor::logging;
using sdbusplus::exception::SdBusError;
using InternalFailure = sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;

constexpr auto SYSTEMD_SERVICE = "org.freedesktop.systemd1";
constexpr auto SYSTEMD_OBJ_PATH = "/org/freedesktop/systemd1";
constexpr auto SYSTEMD_INTERFACE = "org.freedesktop.systemd1.Manager";


NMI::NMI(sdbusplus::bus::bus& bus, const char* path):
    Interface(bus, path),
    bus(bus), objectPath(path)
{

    this->emit_object_added();

}

#if 0
void SoftReset::reset()
{
    using namespace openpower::targeting;
    using namespace openpower::sbe::threadcontrol;
    constexpr auto PROC0_SBE_FIFO_PATH = "/dev/sbefifo1";
    constexpr auto PROC1_SBE_FIFO_PATH = "/dev/sbefifo2";

    Targeting targets;

    // STOP followed by SRESET on all processors,cores & threads on the system
    for (const auto& proc : targets)
    {
        try
        {
            if (proc->getPos() == 0){
                stop(PROC0_SBE_FIFO_PATH,0xFF,0xF);
                sreset(PROC0_SBE_FIFO_PATH, 0xFF, 0xF);
            }
            else if (proc->getPos() == 1){
                stop(PROC1_SBE_FIFO_PATH,0xFF,0xF);
                sreset(PROC1_SBE_FIFO_PATH, 0xFF, 0xF);
            }
            else{
                log<level::ERR>("Found unknown PROC target",
                                entry("PROC=%d", proc->getPos()));
            }

            log<level::INFO>("Executed SBE Reset successfully",
                              entry("PROC=%d", proc->getPos()));
        }
        catch (const std::runtime_error& e)
        {
            log<level::ERR>(e.what());
            elog<InternalFailure>();
            // We want to continue - capturing as much info as possible
        }
    }

}
#endif

void NMI::nmiReset()
{

    log<level::ALERT>("Triggering SRESET");
    auto method = bus.new_method_call(SYSTEMD_SERVICE, SYSTEMD_OBJ_PATH,
                                      SYSTEMD_INTERFACE, "StartUnit");
    method.append("sreset.service", "replace");
    try
    {
        bus.call_noreply(method);
    }
    catch (const SdBusError& e)
    {
        log<level::ALERT>("Error in trying to SRESET service. ");
        report<InternalFailure>();
    }
}

} // namespace proc
} // namespace openpower
