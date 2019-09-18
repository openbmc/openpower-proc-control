#include "bmc_boot_steps.hpp"

#include "util.hpp"

#include <unistd.h>

#include <iostream>

namespace open_power
{
namespace boot
{
namespace bmc_steps
{

int powerOn()
{
    // Poweron the chassis
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
    } while (waitTime < timeout);
    return -1;
}

int StubbedStep()
{
    std::cout << "Step is a stub" << std::endl;
    return 0;
}
} // namespace bmc_steps
} // namespace boot
} // namespace open_power
