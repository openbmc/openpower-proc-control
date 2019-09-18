#include "boot_control.hpp"

#include "bmc_boot_steps.hpp"

namespace openpower
{
namespace boot
{

BMCStepList Control::bmcSteps = {
    {0, {{0, []() { bmc_steps::stubbedStep(); }}}}};

void Control::executeBMCStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return;
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBMCStep(stepMajor, stepMinor);
    }
    return;
}

} // namespace boot
} // namespace openpower
