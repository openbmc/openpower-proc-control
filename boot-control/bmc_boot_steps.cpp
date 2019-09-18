#include "bmc_boot_steps.hpp"
#include "util.hpp"

#include <iostream>
#include <unistd.h>

namespace open_power
{
namespace boot
{
namespace bmc_steps
{

int powerOn()
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

int StubbedStep()
{
    std::cout <<"Step is a stub"<<std::endl;
    return 0;
}
} // bmc_steps
} // boot
} // open_power
