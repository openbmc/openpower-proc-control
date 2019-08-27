#include <iostream>
#include "boot_control.hpp"

namespace open_power
{
namespace boot
{
BmcStepList bmcSteps = {{0,{{0,[](){return BmcExecutor::StartIpl();}},
                            {1,[](){return BmcExecutor::SetRefClock();}},
                            {2,[](){return BmcExecutor::ProcClockTest();}},
                            {3,[](){return BmcExecutor::ProcPrepIpl();}},
                            {4,[](){return BmcExecutor::ProcSelectBootMater();}},
                            {5,[](){return BmcExecutor::SbeConfigUpdate();}},
                            {6,[](){return BmcExecutor::SbeStart();}}}}};

MajorStepsList Control::majorSteps = {{0,{ExecStepType::BMC_STEP,{{0,"poweron"},
                                                         {1,"setrefclock"},
                                                         {2,"procclocktest"}}}},
                             {1,{ExecStepType::SBE_STEP,{{0,"testsbestep"}}}}};

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
                           rc = execute<BmcStep>(stepMajor, stepMinor);
                           break;

        case ExecStepType::SBE_STEP:
                           rc = execute<SbeStep>(stepMajor, stepMinor);
                           break;

        case ExecStepType::HB_STEP:
                           rc = execute<HostbootStep>(stepMajor, stepMinor);
                           break;
     }

     return rc;
}
}
}
