#include "boot_control.hpp"

#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

BmcStepList Control::bmcSteps = {
    {0,
     {{0, []() { return bmc_steps::powerOn(); }},
      {1, []() { return bmc_steps::stubbedStep(); }},
      {2, []() { return bmc_steps::stubbedStep(); }},
      {3, []() { return bmc_steps::stubbedStep(); }},
      {4, []() { return bmc_steps::stubbedStep(); }},
      {5, []() { return bmc_steps::stubbedStep(); }},
      {6, []() { return bmc_steps::sbeConfigUpdate(); }},
      {7, []() { return bmc_steps::startCbs(); }}}}};

MajorStepsList Control::majorSteps = {{0, {{0, "poweron"}}}};

int Control::executeHostStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
}

int Control::executeBmcStep(uint8_t stepMajor, uint8_t stepMinor)
{
    int rc = -1;

    auto mstep = bmcSteps.find(stepMajor);
    if (mstep == bmcSteps.end())
    {
        std::cerr << "Invalid BMC Major step:" << stepMajor << "\n"
                  << std::endl;
        return -1;
    }

    auto istep = mstep->second.find(stepMinor);
    if (istep == mstep->second.end())
    {
        std::cerr << "Invalid BMC Minor step:" << stepMinor << "\n"
                  << std::endl;
    }

    try
    {
        rc = (istep->second)();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error in executing step\n" << std::endl;
        rc = -1;
    }

    return rc;
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

int Control::executeRange(uint8_t startStep, uint8_t endStep)
{
    int rc = -1;

    auto begin_step = majorSteps.find(startStep);
    if (begin_step == majorSteps.end())
    {
        std::cout << "Invalid start step :" << startStep << std::endl;
        return -1;
    }

    auto end_step = majorSteps.find(endStep);
    if (end_step == majorSteps.end())
    {
        std::cout << "Invalid end step :" << endStep << std::endl;
        return -1;
    }
    for (auto iter = begin_step; iter != majorSteps.end(); iter++)
    {
        for (auto miter = iter->second.begin(); miter != iter->second.end();
             miter++)
        {
            std::cout << "Executing:" << miter->second << std::endl;
            rc = executeStep(iter->first, miter->first);
            if (rc)
            {
                return rc;
            }
        }

        if (endStep == iter->first)
        {
            break;
        }
    }
    return rc;
}
} // namespace boot
} // namespace open_power
