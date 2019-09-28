extern "C" {
#include <libpdbg.h>
}

#include "bmc_boot_steps.hpp"
#include "pdbg.hpp"
#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <p9_nv_ref_clk_enable.H>
#include <p9_setup_sbe_config.H>
#include <p9_start_cbs.H>
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
            // Init targets only if chassin is on
            pdbg::initTargets();
            return;
        }
        std::this_thread::sleep_for(sleepTime);
        sleepTime = std::chrono::seconds(2);
    }

    log<level::ERR>("Timeout encountered while waiting for chassis on");
    elog<InternalFailure>();
}

void sbeConfigUpdate()
{
    struct pdbg_target* pib;

    // TODO - Curently no attribute support available to check master
    // processor. First processor in the list is the master processor.
    // This should change, once master processor attribute is enabled
    pdbg_for_each_class_target("pib", pib)
    {
        if (pdbg_target_index(pib) == 0)
        {
            int rc = -1;
            if ((rc = p9_setup_sbe_config(pib)) != 0)
            {
                log<level::ERR>("p9_setup_sbe_config failed",
                                entry("ERRORCODE=%d", rc),
                                entry("TARGETNAME=%s", pdbg_target_name(pib)));
                elog<InternalFailure>();
            }
            break;
        };
    }
}

void startSbe()
{
    int rc = -1;
    struct pdbg_target* pib;

    // TODO - Curently no attribute support available to check master
    // processor. First processor in the list is the master processor.
    // This should change, once master processor attribute is enabled
    pdbg_for_each_class_target("pib", pib)
    {
        if (pdbg_target_index(pib) == 0)
        {
            if ((rc = p9_start_cbs(pib, true)) != 0)
            {
                log<level::ERR>("p9_start_cbs failed",
                                entry("ERRORCODE=%d", rc),
                                entry("TARGETNAME=%s", pdbg_target_name(pib)));
                elog<InternalFailure>();
            }
            if ((rc = p9_nv_ref_clk_enable(pib)) != 0)
            {
                log<level::ERR>("p9_nv_ref_clk_enable failed",
                                entry("ERRORCODE=%d", rc),
                                entry("TARGETNAME=%s", pdbg_target_name(pib)));
                elog<InternalFailure>();
            }
            break;
        }
    }
}

void stubbedStep()
{
    log<level::INFO>("Step is a stub");
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
