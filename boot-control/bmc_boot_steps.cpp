extern "C" {
#include <libpdbg.h>
}

#include "config.h"

#include "bmc_boot_steps.hpp"
#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#ifdef VERSION
#define PKG_VERSION VERSION
#undef VERSION
#endif

#if P10_CHIP
#include <p10_start_cbs.H>
#else
#include <p9_nv_ref_clk_enable.H>
#include <p9_setup_sbe_config.H>
#include <p9_start_cbs.H>
#endif

#ifdef PKG_VERSION
#define VERSION PKG_VERSION
#undef PKG_VERSION
#endif

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
            util::initTargets();
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

    pdbg_for_each_class_target("pib", pib)
    {
        int rc = -1;
#if P10_CHIP
        // TODO
#else
        if ((rc = p9_setup_sbe_config(pib)) != 0)
#endif
        {
            log<level::ERR>("sbeConfigUpdate is failed",
                            entry("RETURNCODE=%d", rc));
            elog<InternalFailure>();
        }
        return;
    }
}

void startCbs()
{
    int rc = -1;
    struct pdbg_target* pib;

    pdbg_for_each_class_target("pib", pib)
    {
#if P10_CHIP
        if ((rc = p10_start_cbs(pib)) != 0)
#else
        if ((rc = p9_start_cbs(pib, true)) != 0)
#endif
        {
            log<level::ERR>("startCbs is failed", entry("RETURNCODE=%d", rc));
            elog<InternalFailure>();
        }
#if P10_CHIP
        return;
        // TODO
#else
        else
        {
            if ((rc = p9_nv_ref_clk_enable(pib)) != 0)
            {
                log<level::ERR>("nvRefClkEnable is failed",
                                entry("RETURNCODE=%d", rc));
                elog<InternalFailure>();
            }
            return;
        }
#endif
    }
}

void stubbedStep()
{
    std::cout << "Step is a stub" << std::endl;
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
