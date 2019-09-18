#include "boot_control.hpp"

#include "bmc_boot_steps.hpp"

namespace openpower
{
namespace boot
{

BmcStepList Control::bmcSteps = {
    {0, {{0, []() { bmc_steps::stubbedStep(); }}}}};

void Control::executeBmcStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return;
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBmcStep(stepMajor, stepMinor);
    }
    return;
}

} // namespace boot
} // namespace openpower
