#include "boot_control.hpp"
#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

BmcStepList Control::bmcSteps =
            {{0,{{0,[](){return bmc_steps::powerOn();}},
                 {1,[](){return bmc_steps::StubbedStep();}},
                 {2,[](){return bmc_steps::StubbedStep();}},
                 {3,[](){return bmc_steps::StubbedStep();}},
                 {4,[](){return bmc_steps::StubbedStep();}},
                 {5,[](){return bmc_steps::StubbedStep();}},
                 {6,[](){return bmc_steps::StubbedStep();}},
                 {7,[](){return bmc_steps::StubbedStep();}}}}};

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

int Control::executeRange(uint8_t startStep, uint8_t endStep)
{
    int rc = -1;

    auto begin_step = majorSteps.find(startStep);
    if (begin_step == majorSteps.end())
    {
        std::cout << "Invalid start step :"<< startStep <<std::endl;
        return -1;
    }

    auto end_step = majorSteps.find(endStep);
    if (end_step == majorSteps.end())
    {
        std::cout << "Invalid end step :"<< endStep <<std::endl;
        return -1;
    }
    for (auto iter = begin_step; iter != majorSteps.end(); iter++)
    {
        for (auto miter = iter->second.minorStepList.begin();
             miter!= iter->second.minorStepList.end(); miter++)
        {
            std::cout << "Executing:"<<miter->second<<std::endl;
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
} // boot
} // open_power
