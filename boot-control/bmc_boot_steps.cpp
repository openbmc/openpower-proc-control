#include "bmc_boot_steps.hpp"

#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <unistd.h>

#include <chrono>
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>

namespace openpower
{
namespace boot
{
namespace bmc_steps
{
using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

void powerOn()
{
    // Poweron the chassis
    util::chassisPowerOn();

    auto sleepTime = std::chrono::seconds(5);
    auto timeout = std::chrono::seconds(60);
    auto now = std::chrono::system_clock::now();
    // TODO: change this to signal based wait for chassis poweron
    while (std::chrono::system_clock::now() < (now + timeout))
    {
        if (util::isChassisOn())
        {
            return;
        }
        std::this_thread::sleep_for(sleepTime);
        sleepTime = std::chrono::seconds(2);
    }

    log<level::ERR>("Timeout encountered while waiting for chassis on");
    elog<InternalFailure>();
}

void stubbedStep()
{
    log<level::INFO>("Step is a stub");
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
