#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{

int BmcExecutor::StartIpl()
{
    return StubbedStep("StartIpl");
}

int BmcExecutor::SetRefClock()
{
    return StubbedStep("SetRefClock");
}

int BmcExecutor::ProcClockTest()
{
    return StubbedStep("ProcClockTest");
}

int BmcExecutor::ProcPrepIpl()
{
     return StubbedStep("ProcPrepIpl");
}

int BmcExecutor::ProcSelectBootMater()
{
    return StubbedStep("ProcSelectBootMater");
}

int BmcExecutor::SbeConfigUpdate()
{
    return StubbedStep("SbeConfigUpdate");
}

int BmcExecutor::SbeStart()
{
    return StubbedStep("SbeConfigUpdate");
}

int BmcExecutor::StubbedStep(const char *des)
{

    std::cout <<"Step "<<des<<" is a stub"<<std::endl;
    return 0;
}


}
}
