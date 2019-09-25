extern "C" {
#include <libpdbg.h>
}

#include "bmc_boot_steps.hpp"
#include "pdbg_wrapper.hpp"
#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <p10_start_cbs.H>
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
    auto timeout = std::chrono::seconds(20);
    auto now = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() < (now + timeout))
    {
        if (util::isChassisOn())
        {
            // Init targets only if chassin is on
            util::pdbg::initTargets();
            return;
        }
        std::this_thread::sleep_for(sleepTime);
        sleepTime = std::chrono::seconds(2);
    }

    log<level::ERR>("Timeout encountered while waiting for chassis on");
    elog<InternalFailure>();
}

void startSbe()
{
    int rc = -1;
    struct pdbg_target* pib;

    // TODO - Curently no attribute support available to chcek master
    // processor. First processor in the list is the master processor.
    // This should change, once master processor attribute is enabled
    pdbg_for_each_class_target("pib", pib)
    {
        if ((rc = p10_start_cbs(pib)) != 0)
        {
            log<level::ERR>("startSbe is failed", entry("RETURNCODE=%d", rc));
            elog<InternalFailure>();
        }
        return;
    }
}

void stubbedStep()
{
    std::cout << "Step is a stub" << std::endl;
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
