#include "boot_control.hpp"
#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

BmcStepList Control::bmcSteps = {
    {0,
     {{0, []() { return bmc_steps::StubbedStep(); }},
      {1, []() { return bmc_steps::StubbedStep(); }},
      {2, []() { return bmc_steps::StubbedStep(); }},
      {3, []() { return bmc_steps::StubbedStep(); }},
      {4, []() { return bmc_steps::StubbedStep(); }},
      {5, []() { return bmc_steps::StubbedStep(); }},
      {6, []() { return bmc_steps::StubbedStep(); }},
      {7, []() { return bmc_steps::StubbedStep(); }}}}};

int Control::executeHostStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
}

int Control::executeBmcStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
}

int Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    int rc = -1;
    if (stepMajor == 0)
    {
        rc = executeBmcStep(stepMajor, stepMinor);
    }
    else
    {
        rc = executeHostStep(stepMajor, stepMinor);
    }
    return rc;
}

} // namespace boot
} // namespace open_power
