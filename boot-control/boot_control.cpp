#include "boot_control.hpp"

#include "bmc_boot_steps.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <iostream>
#include <phosphor-logging/elog-errors.hpp>

namespace openpower
{
namespace boot
{
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

BMCStepList Control::bmcSteps = {{0, {{7, []() { bmc_steps::startSbe(); }}}}};

MajorStepsList Control::majorSteps = {{0, {{7, "startsbe"}}},
                                      {1, {{0, "stub"}}}};

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

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBMCStep(stepMajor, stepMinor);
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
