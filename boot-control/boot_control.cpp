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

BMCStepList Control::bmcSteps = {{0, {{0, []() { bmc_steps::powerOn(); }}}}};

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

} // namespace boot
} // namespace openpower
