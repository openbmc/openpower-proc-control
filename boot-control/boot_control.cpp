extern "C" {
#include <libpdbg.h>
}

#include "config.h"

#include "bmc_boot_steps.hpp"
#include "boot_control.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <phosphor-logging/elog-errors.hpp>
namespace openpower
{
namespace boot
{
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

BMCStepList Control::bmcSteps = {{0,
                                  {{0, []() { bmc_steps::powerOn(); }},
                                   {7, []() { bmc_steps::startSbe(); }}}}};

void Control::executeBMCStep(uint8_t stepMajor, uint8_t stepMinor)
{
    auto mstep = bmcSteps.find(stepMajor);
    if (mstep == bmcSteps.end())
    {
        log<level::ERR>("Invalid BMC Major step",
                        entry("MAJORSTEP=%d", stepMajor));
        elog<InternalFailure>();
    }

    auto istep = mstep->second.find(stepMinor);
    if (istep == mstep->second.end())
    {
        log<level::ERR>("Invalid BMC Minor step",
                        entry("MINORSTEP=%d", stepMinor));
        elog<InternalFailure>();
    }

    // Execute the BMC step
    (istep->second)();
}

void Control::executeHostStep(uint8_t stepMajor, uint8_t stepMinor)
{
    // TODO This type of direct execution of istep chipop to SBE
    // is only until common HWP is available to trigger step in
    // both SBE and Hostboot.
    int rc;
    if ((stepMajor > 0) && (stepMajor < 6))
    {
        struct pdbg_target* target;
        pdbg_for_each_class_target("pib", target)
        {
            if (pdbg_target_status(target) != PDBG_TARGET_ENABLED)
            {
                continue;
            }
            if ((rc = sbe_istep(target, stepMajor, stepMinor)) != 0)
            {
                log<level::ERR>("Error executing SBE step", entry("RC=%d", rc));
                elog<InternalFailure>();
            }
        }
    }
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBMCStep(stepMajor, stepMinor);
    }
}

void Control::loadSteps()
{
    std::ifstream stepsFile(BOOT_STEP_FILE);
    if (!stepsFile.is_open())
    {
        log<level::ERR>("Failed to open boot step file",
                        entry("BOOT_STEP_FILE=%s", BOOT_STEP_FILE));
        elog<InternalFailure>();
    }

    auto data = nlohmann::json::parse(stepsFile, nullptr, false);
    if (data.is_discarded())
    {
        log<level::ERR>("Invalid json file",
                        entry("JSONFILE=%s", BOOT_STEP_FILE));
        elog<InternalFailure>();
    }

    try
    {
        for (const auto& entry : data)
        {
            MajorStepNumber majorStep =
                entry.at("MajorStep").get<MajorStepNumber>();
            auto minorSteps = entry.at("MinorSteps");
            MinorStepList minorStepList;
            for (const auto& minorStep : minorSteps)
            {
                minorStepList.insert(std::make_pair(
                    minorStep.at("MinorStep").get<MinorStepNumber>(),
                    minorStep.at("StepName").get<StepName>()));
            }
            majorSteps.insert(std::make_pair(majorStep, minorStepList));
        }
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Error in parsing json file",
                        entry("JSONFILE=%s", BOOT_STEP_FILE));
        elog<InternalFailure>();
    }
}

void Control::executeRange(uint8_t startStep, uint8_t endStep)
{
    if (endStep < startStep)
    {
        log<level::ERR>("Start step cannot be grater than end step",
                        entry("STARTSTEP=%d", startStep),
                        entry("ENDSTEP=%d", endStep));
        elog<InternalFailure>();
    }

    try
    {
        loadSteps();
    }
    catch (const InternalFailure& e)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Error in loading steps");
        elog<InternalFailure>();
    }

    auto begin_step = majorSteps.find(startStep);
    if (begin_step == majorSteps.end())
    {
        log<level::ERR>("Invalid start step", entry("STARTSTEP=%d", startStep));
        elog<InternalFailure>();
    }

    auto end_step = majorSteps.find(endStep);
    if (end_step == majorSteps.end())
    {
        log<level::ERR>("Invalid end step", entry("ENDSTEP=%d", endStep));
        elog<InternalFailure>();
    }

    for (auto iter = begin_step; iter != majorSteps.end(); iter++)
    {
        for (auto& mStep : iter->second)
        {
            std::cout << "Executing:" << mStep.second << std::endl;
            executeStep(iter->first, mStep.first);
        }

        if (endStep == iter->first)
        {
            break;
        }
    }
}
} // namespace boot
} // namespace openpower
