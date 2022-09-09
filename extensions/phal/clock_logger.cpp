/**
 * Copyright © 2022 IBM Corporation
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

#include "extensions/phal/clock_logger.hpp"

#include "util.hpp"

#include <attributes_info.H>
#include <libphal.H>

#include <phosphor-logging/log.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

#include <chrono>

using namespace phosphor::logging;
using namespace openpower::pel;

namespace openpower::phal::clock
{
constexpr auto CLOCK_DAILY_LOGGER_TIMEOUT_IN_HOUR = 24;

Manager::Manager(const sdeventplus::Event& event) :
    _event(event), timer(event, std::bind(&Manager::timerExpired, this))

{
    try
    {
        // Create clock data log.
        createClockDataLog();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(fmt::format("createClockDataLog exception",
                                    "EXCEPTION={}", e.what())
                            .c_str());
    }

    addTimer();
}

void Manager::addTimer()
{
    // Set timer for 24 hours.
    timer.restart(std::chrono::hours(CLOCK_DAILY_LOGGER_TIMEOUT_IN_HOUR));
}

void Manager::timerExpired()
{
    log<level::INFO>("Clock daily logging started");

    try
    {
        // Create clock data log.
        createClockDataLog();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(fmt::format("createClockDataLog exception",
                                    "EXCEPTION={}", e.what())
                            .c_str());
    }
}

void Manager::createClockDataLog()
{
    // check chassis power state.
    if (openpower::util::getChassisPowerState() !=
        "xyz.openbmc_project.State.Chassis.PowerState.On")
    {
        log<level::WARNING>(
            "The chassis power state is not ON, Skipping clock data logging");
        return;
    }

    // Data logger storage
    FFDCData clockDataLog;

    // pdbg initialisation
    openpower::phal::pdbg::init();

    struct pdbg_target* procTarget;
    ATTR_HWAS_STATE_Type hwasState;
    pdbg_for_each_class_target("proc", procTarget)
    {

        if (DT_GET_PROP(ATTR_HWAS_STATE, procTarget, hwasState))
        {
            log<level::ERR>(
                fmt::format("({})Could not read HWAS_STATE attribute",
                            pdbg_target_path(procTarget))
                    .c_str());
            continue;
        }
        if (!hwasState.present)
        {
            continue;
        }

        auto index = std::to_string(pdbg_target_index(procTarget));

        // update functional State
        std::string funState = "Non Functional";

        if (hwasState.functional)
        {
            funState = "Functional";
        }
        std::stringstream ssState;
        ssState << "Proc" << index;
        clockDataLog.push_back(std::make_pair(ssState.str(), funState));

        // update location code information
        ATTR_LOCATION_CODE_Type locationCode;
        memset(&locationCode, '\0', sizeof(locationCode));
        try
        {
            openpower::phal::pdbg::getLocationCode(procTarget, locationCode);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>(
                fmt::format("getLocationCode on ({}) thrown exception({})",
                            pdbg_target_path(procTarget), e.what())
                    .c_str());
        }
        std::stringstream ssLoc;
        ssLoc << "Proc" << index << " Location Code";
        clockDataLog.push_back(std::make_pair(ssLoc.str(), locationCode));

        // Update Processor EC level
        ATTR_EC_Type ecVal = 0;
        if (DT_GET_PROP(ATTR_EC, procTarget, ecVal))
        {
            log<level::ERR>("Could not read ATTR_EC  attribute");
        }
        std::stringstream ssEC;
        ssEC << "Proc" << index << " EC";

        std::stringstream ssECVal;
        ssECVal << "0x" << std::setfill('0') << std::setw(10) << std::hex
                << (uint16_t)ecVal;
        clockDataLog.push_back(std::make_pair(ssEC.str(), ssECVal.str()));

        // Add CFAM register information.
        addCFAMData(procTarget, clockDataLog);
    }

    openpower::pel::createPEL("org.open_power.PHAL.Info.ClockDailyLog",
                              clockDataLog);
}

void Manager::addCFAMData(struct pdbg_target* proc,
                          openpower::pel::FFDCData& clockDataLog)
{

    // collect Processor CFAM register data
    std::vector<int> procCFAMAddr = {0x1007, 0x2804, 0x2810, 0x2813, 0x2814,
                                     0x2815, 0x2816, 0x281D, 0x281E};

    auto index = std::to_string(pdbg_target_index(proc));

    for (int addr : procCFAMAddr)
    {
        auto val = 0xDEADBEEF;
        try
        {
            val = openpower::phal::pdbg::getCFAM(proc, addr);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>(
                fmt::format("getCFAM on ({}) thrown exception({}): Addr ({})",
                            pdbg_target_path(proc), e.what(), addr)
                    .c_str());
        }
        std::stringstream ssData;
        ssData << "0x" << std::setfill('0') << std::setw(8) << std::hex << val;
        std::stringstream ssAddr;
        ssAddr << "Proc" << index << " REG 0x" << std::hex << addr;
        // update the data
        clockDataLog.push_back(make_pair(ssAddr.str(), ssData.str()));
    }
}

} // namespace openpower::phal::clock
