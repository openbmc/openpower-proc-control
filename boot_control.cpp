#include <iostream>
#include "boot_control.hpp"

namespace open_power
{
namespace boot
{
BmcStepList bmcSteps = {{0,{{0,[](){BmcExecutor::StartIpl();}},
                            {1,[](){BmcExecutor::SetRefClock();}},
                            {2,[](){BmcExecutor::ProcClockTest();}},
                            {3,[](){BmcExecutor::ProcPrepIpl();}},
                            {4,[](){BmcExecutor::ProcSelectBootMater();}},
                            {5,[](){BmcExecutor::SbeConfigUpdate();}},
                            {6,[](){BmcExecutor::SbeStart();}}}}};

MajorStepsList majorSteps = {{0,ExecStepType::BMC_STEP}};

void BmcStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
        bmcSteps[stepMajor][stepMinor]();
}

void SbeStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
}

void HostbootStep::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    ExecStepType execType = ExecStepType::BMC_STEP;
    try
    {
        execType = majorSteps[stepMajor];
    }

    catch (const std::out_of_range& oor)
    {
        //Log error
        //Throw error
        std::cerr << "Invalid major step"<<std::endl;
    }

    switch (execType)
    {
        case ExecStepType::BMC_STEP:
                           execute<BmcStep>(stepMajor, stepMinor);

        case ExecStepType::SBE_STEP:
                           execute<SbeStep>(stepMajor, stepMinor);
        case ExecStepType::HB_STEP:
                           execute<HostbootStep>(stepMajor, stepMinor);
     }
}
}
}
