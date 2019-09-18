#include "boot_control.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

MajorStepsList Control::majorSteps =
                                 {{0,{ExecStepType::BMC_STEP,{{0,"poweron"}}}}};

int BmcStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
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
} // boot
} // open_power
