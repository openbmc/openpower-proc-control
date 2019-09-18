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

BmcStepList Control::bmcSteps = {{0, {{0, []() { bmc_steps::powerOn(); }}}}};

MajorStepsList Control::majorSteps = {{0, {{0, "poweron"}}},
                                      {1, {{0, "sbestub"}}}};

void Control::executeBmcStep(uint8_t stepMajor, uint8_t stepMinor)
{
    auto mstep = bmcSteps.find(stepMajor);
    if (mstep == bmcSteps.end())
    {
        log<level::ERR>("Invalid BMC Major step",
                        entry("MajorStep=%d", stepMajor));
        elog<InternalFailure>();
    }

    auto istep = mstep->second.find(stepMinor);
    if (istep == mstep->second.end())
    {
        log<level::ERR>("Invalid BMC Minor step",
                        entry("MinorStep=%d", stepMinor));
        elog<InternalFailure>();
    }

    // Execute the BMC step
    (istep->second)();
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBmcStep(stepMajor, stepMinor);
    }
}

void Control::executeRange(uint8_t startStep, uint8_t endStep)
{
    auto begin_step = majorSteps.find(startStep);
    if (begin_step == majorSteps.end())
    {
        log<level::ERR>("Invalid start step", entry("StartStep=%d", startStep));
        elog<InternalFailure>();
    }

    auto end_step = majorSteps.find(endStep);
    if (end_step == majorSteps.end())
    {
        log<level::ERR>("Invalid end step", entry("endStep=%d", endStep));
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
