#include "boot_control.hpp"
#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

BmcStepList bmcSteps = {{0,{{0,[](){return BmcExecutor::PowerOn();}},
                           {1,[](){return BmcExecutor::StartIpl();}},
                           {2,[](){return BmcExecutor::SetRefClock();}},
                           {3,[](){return BmcExecutor::ProcClockTest();}},
                           {4,[](){return BmcExecutor::ProcPrepIpl();}},
                           {5,[](){return BmcExecutor::ProcSelectBootMater();}},
                           {6,[](){return BmcExecutor::SbeConfigUpdate();}},
                           {7,[](){return BmcExecutor::SbeStart();}}}}};

MajorStepsList Control::majorSteps = {{0,{ExecStepType::BMC_STEP,{{0,"poweron"},
                                                     {1,"startipl"},
                                                     {2,"setrefclock"},
                                                     {3,"procclocktest"},
                                                     {4,"procprepipl"},
                                                     {5,"procselectbootmaster"},
                                                     {6,"sbeconfigupdate"},
                                                     {7,"sbestart"}}}}};

int BmcStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return bmcSteps[stepMajor][stepMinor]();
}

int SbeStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
   return 0;
}

int HostbootStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    
    return 0;
}

int Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    ExecStepType execType = ExecStepType::BMC_STEP;
    try
    {
        execType = majorSteps[stepMajor].stepType;
    }

    catch (const std::out_of_range& oor)
    {
        std::cerr << "Invalid major step"<<std::endl;
        return -1;
    }

    int rc = -1;

    switch (execType)
    {
        case ExecStepType::BMC_STEP:
                           rc = bmcExec->executeStep(stepMajor, stepMinor);
                           break;

        case ExecStepType::SBE_STEP:
                           rc = sbeExec->executeStep(stepMajor, stepMinor);
                           break;

        case ExecStepType::HB_STEP:
                           rc = hbExec->executeStep(stepMajor, stepMinor);
                           break;
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
