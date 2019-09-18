#include "bmc_boot_steps.hpp"
#include "util.hpp"

#include <iostream>
#include <unistd.h>

namespace open_power
{
namespace boot
{

int BmcExecutor::PowerOn()
{
    //Poweron the chassis
    util::chassisPowerOn();
    uint32_t sleepTime = 5;
    uint32_t timeout = 20;
    uint32_t waitTime = 0;

    do
    {
        if (util::isChassisOn())
        {
            return 0;
        }
        sleep(sleepTime);
        waitTime += sleepTime;
        sleepTime = 2;
    }
    while (waitTime < timeout);
    return -1;
}
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
    return StubbedStep("SbeStart");
}

int BmcExecutor::StubbedStep(const char *des)
{

    std::cout <<"Step "<<des<<" is a stub"<<std::endl;
    return 0;
}


}
}
