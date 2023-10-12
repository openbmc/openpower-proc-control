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

#include <phosphor-logging/lg2.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

#include <chrono>

using namespace openpower::pel;

PHOSPHOR_LOG2_USING;

namespace openpower::phal::clock
{
constexpr auto CLOCK_DAILY_LOGGER_TIMEOUT_IN_HOUR = 24;

Manager::Manager(const sdeventplus::Event& event) :
    _event(event), timer(event, std::bind(&Manager::timerExpired, this))

{
    try
    {
        // pdbg initialisation
        openpower::phal::pdbg::init();

        // Create clock data log.
        createClockDataLog();
    }
    catch (const std::exception& e)
    {
        error("Clock Data Log exception ({ERROR})", "ERROR", e);
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
    info("Clock daily logging started");

    try
    {
        // Create clock data log.
        createClockDataLog();
    }
    catch (const std::exception& e)
    {
        error("createClockDataLog exception ({ERROR})", "ERROR", e);
    }
}

void Manager::createClockDataLog()
{
    // Data logger storage
    FFDCData clockDataLog;

    struct pdbg_target* procTarget;
    ATTR_HWAS_STATE_Type hwasState;
    pdbg_for_each_class_target("proc", procTarget)
    {
        if (DT_GET_PROP(ATTR_HWAS_STATE, procTarget, hwasState))
        {
            error("{TARGET} Could not read HWAS_STATE attribute", "TARGET",
                  pdbg_target_path(procTarget));
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
            error("getLocationCode on {TARGET} thrown exception ({ERROR})",
                  "TARGET", pdbg_target_path(procTarget), "ERROR", e);
        }
        std::stringstream ssLoc;
        ssLoc << "Proc" << index << " Location Code";
        clockDataLog.push_back(std::make_pair(ssLoc.str(), locationCode));

        // Update Processor EC level
        ATTR_EC_Type ecVal = 0;
        if (DT_GET_PROP(ATTR_EC, procTarget, ecVal))
        {
            error("Could not read ATTR_EC  attribute");
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

    // Add clock register information
    addClockRegData(clockDataLog);

    openpower::pel::createPEL("org.open_power.PHAL.Info.ClockDailyLog",
                              clockDataLog, Severity::Informational);
}

void Manager::addCFAMData(struct pdbg_target* proc,
                          openpower::pel::FFDCData& clockDataLog)
{
    // collect Processor CFAM register data
    const std::vector<int> procCFAMAddr = {
        0x1007, 0x2804, 0x2810, 0x2813, 0x2814, 0x2815, 0x2816, 0x281D, 0x281E};

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
            error("getCFAM on {TARGET} thrown exception({ERROR}): Addr ({REG})",
                  "TARGET", pdbg_target_path(proc), "ERROR", e, "REG", addr);
        }
        std::stringstream ssData;
        ssData << "0x" << std::setfill('0') << std::setw(8) << std::hex << val;
        std::stringstream ssAddr;
        ssAddr << "Proc" << index << " REG 0x" << std::hex << addr;
        // update the data
        clockDataLog.push_back(make_pair(ssAddr.str(), ssData.str()));
    }
}

void Manager::addClockRegData(openpower::pel::FFDCData& clockDataLog)
{
    info("Adding clock register information to daily logger");

    struct pdbg_target* clockTarget;
    pdbg_for_each_class_target("oscrefclk", clockTarget)
    {
        ATTR_HWAS_STATE_Type hwasState;
        if (DT_GET_PROP(ATTR_HWAS_STATE, clockTarget, hwasState))
        {
            error("({TARGET}) Could not read HWAS_STATE attribute", "TARGET",
                  pdbg_target_path(clockTarget));
            continue;
        }

        if (!hwasState.present)
        {
            continue;
        }

        std::string funState = "Non Functional";

        if (hwasState.functional)
        {
            funState = "Functional";
        }

        auto index = std::to_string(pdbg_target_index(clockTarget));

        std::stringstream ssState;
        ssState << "Clock" << index;
        clockDataLog.push_back(std::make_pair(ssState.str(), funState));

        // Add clcok device path information
        std::stringstream ssName;
        ssName << "Clock" << index << " path";
        clockDataLog.push_back(
            std::make_pair(ssName.str(), pdbg_target_path(clockTarget)));

        auto status = pdbg_target_probe(clockTarget);
        if (status != PDBG_TARGET_ENABLED)
        {
            continue;
        }

        // Update Buffer with clock I2C register data.
        auto constexpr I2C_READ_SIZE = 0x08;
        auto constexpr I2C_ADDR_MAX = 0xFF;

        for (auto addr = 0; addr <= I2C_ADDR_MAX; addr += I2C_READ_SIZE)
        {
            std::stringstream ssData;

            uint8_t data[0x8];
            auto i2cRc = i2c_read(clockTarget, 0, addr, I2C_READ_SIZE, data);
            if (i2cRc)
            {
                error("({TARGET}) I2C read error({ERROR}) reported {ADDRESS} ",
                      "TARGET", pdbg_target_path(clockTarget), "ERROR", i2cRc,
                      "ADDRESS", addr);
                continue;
            }

            for (auto i = 0; i < I2C_READ_SIZE; i++)
            {
                ssData << " " << std::hex << std::setfill('0') << std::setw(2)
                       << (uint16_t)data[i];
            }
            std::stringstream ssAddr;
            ssAddr << "Clock" << index << "_0x" << std::hex << std::setfill('0')
                   << std::setw(2) << addr;
            clockDataLog.push_back(make_pair(ssAddr.str(), ssData.str()));
        }
    }
}

} // namespace openpower::phal::clock
