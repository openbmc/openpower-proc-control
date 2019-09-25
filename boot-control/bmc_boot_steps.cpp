extern "C" {
#include <libpdbg.h>
}

#include "bmc_boot_steps.hpp"
#include "util.hpp"

#include <p9_nv_ref_clk_enable.H>
#include <p9_setup_sbe_config.H>
#include <p9_start_cbs.H>
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
            // Init targets only if chassin is on
            if (0 != util::initTargets())
            {
                return -1;
            }
            return 0;
        }
        std::this_thread::sleep_for(sleepTime);
        sleepTime = std::chrono::seconds(2);
    }

    return -1;
}

int sbeConfigUpdate()
{
    int rc = -1;
    struct pdbg_target* pib;

    pdbg_for_each_class_target("pib", pib)
    {
        rc = p9_setup_sbe_config(pib);
        return rc;
    }
    return rc;
}

int startCbs()
{
    int rc = -1;
    struct pdbg_target* pib;

    pdbg_for_each_class_target("pib", pib)
    {
        if ((rc = p9_start_cbs(pib, true)) != 0)
        {
            return rc;
        }
        return p9_nv_ref_clk_enable(pib);
    }
    return rc;
}

int stubbedStep()
{
    std::cout << "Step is a stub" << std::endl;
    return 0;
}

} // namespace bmc_steps
} // namespace boot
} // namespace open_power
