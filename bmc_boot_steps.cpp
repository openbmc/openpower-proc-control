#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

void BmcExecutor::StartIpl()
{
    StubbedStep("StartIpl");
}

void BmcExecutor::SetRefClock()
{
    StubbedStep("SetRefClock");
}

void BmcExecutor::ProcClockTest()
{
    StubbedStep("ProcClockTest");
}

void BmcExecutor::ProcPrepIpl()
{
     StubbedStep("ProcPrepIpl");
}

void BmcExecutor::ProcSelectBootMater()
{
    StubbedStep("ProcSelectBootMater");
}

void BmcExecutor::SbeConfigUpdate()
{
    StubbedStep("SbeConfigUpdate");
}

void BmcExecutor::SbeStart()
{
   StubbedStep("SbeStart");
}

void BmcExecutor::StubbedStep(const char *des)
{

    std::cout <<"Step "<<des<<" is a stub"<<std::endl;
}


}
}
