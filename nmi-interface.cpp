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
#include <libpdbg.h>
#include "targeting.hpp"
#include <iostream>

namespace openpower
{
namespace proc
{

using namespace phosphor::logging;
using namespace std;
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

void NMI::nmiReset()
{
    struct pdbg_target *target;
    enum pdbg_target_status status;
    int rc = 0;
    log<level::ALERT>("Triggering stop/sreset on all threads");

    //stop all the threads
    pdbg_for_each_class_target("thread", target) 
    {
        log<level::ALERT>("In Loop");
        status = pdbg_target_probe(target);
        if (status ==  PDBG_TARGET_NONEXISTENT)
            continue;
        
        log<level::ALERT>("Triggering stop on threads");
        rc=ram_stop_thread(target);
        cout <<"RC code for stop:" <<rc <<endl;
        log<level::ALERT>("Triggering sreset on threads");
        rc=ram_sreset_thread(target);
        cout <<"RC code for sreset:" <<rc <<endl;
    }

    /* 
    //sreset on all the threads
    log<level::ALERT>("Triggering sreset on all threads");
    pdbg_for_each_class_target("thread", target) 
    {
        if (pdbg_target_status(target) != PDBG_TARGET_ENABLED)
            log<level::ALERT>("Target not enabled");
            continue;

        rc=ram_sreset_thread(target);
        log<level::ALERT>("Target not enabled, rc=%d", rc);
    }
    */
}
} // namespace proc
} // namespace openpower
