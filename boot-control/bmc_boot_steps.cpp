#include "bmc_boot_steps.hpp"

#include "util.hpp"

#include <unistd.h>

#include <chrono>
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

    auto sleepTime = std::chrono::seconds(5);
    auto timeout = std::chrono::seconds(20);
    auto now = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() < (now + timeout))
    {
        if (util::isChassisOn())
        {
            return 0;
        }
        std::this_thread::sleep_for(sleepTime);
        sleepTime = std::chrono::seconds(2);
    }

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
