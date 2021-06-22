extern "C"
{
#include "libpdbg.h"
}

#include "cfam_access.hpp"
#include "common_utils.hpp"
#include "p10_cfam.hpp"
#include "phalerror/create_pel.hpp"
#include "procedures/phal/common_utils.hpp"
#include "registration.hpp"
#include "targeting.hpp"

#include <phosphor-logging/log.hpp>

#include <cstdio>
#include <fstream>
#include <memory>

namespace openpower
{
namespace phal
{

using namespace openpower::cfam::access;
using namespace openpower::cfam::p10;
using namespace openpower::targeting;
using namespace phosphor::logging;

/**
 * This is the backup plan to ensuring the host is not running before the
 * BMC issues a power off to the system. Prior to this procedure being called,
 * the BMC has tried all other communication mechanisms to talk with the host
 * and they have failed. The design is that the host firmware will write the
 * value 0xA5000001 to Mailbox scratch register 12 when they are up and running
 * to a point where communication to the BMC is no longer required to function.
 * On a power off or shutdown this register is cleared by the host and BMC
 * firmware. If the BMC sees the 0xA5000001 pattern in the scratch register
 * then it assumes the host is running and will leave power on to the system.
 */
void checkHostRunning()
{
    struct pdbg_target* procTarget;
    log<level::INFO>("Entering checkHostRunning");

    try
    {
        phal_init();
    }
    catch (std::exception& ex)
    {
        log<level::ERR>("Exception raised during init PHAL",
                        entry("EXCEPTION=%s", ex.what()));
        throw std::runtime_error("PHAL initialization failed");
    }

    pdbg_for_each_class_target("proc", procTarget)
    {
        log<level::INFO>("Checking a proc");
        // Only check the master proc
        if (!isMasterProc(procTarget))
        {
            log<level::INFO>("Not the master proc");
            continue;
        }

        log<level::INFO>("Found the master proc");

        uint32_t val = 0;
        auto rc = fsi_read(procTarget, P10_SCRATCH_REG_12, &val);
        if (!rc)
        {
            log<level::ERR>("failed to read reset reload cfam",
                            entry("RC=%u", rc));

            // Assume not running
            return;
        }
        log<level::INFO>("Read the CFAM, no error");

        constexpr uint32_t HOST_RUNNING_INDICATION = 0xA5000001;
        if (val != HOST_RUNNING_INDICATION)
        {
            log<level::INFO>("CFAM read indicates host is not running",
                             entry("CFAM=0x%X", val));
            return;
        }
        // This is not good. Normal communication path to host did not work but
        // CFAM indicates host is running.
        log<level::ERR>("CFAM read indicates host is running");

        // Create an error so user knows system is in a bad state
        openpower::pel::createHostRunningPEL();

        // Create file for host instance and create in filesystem to
        // indicate to services that host is running
        constexpr auto HOST_RUNNING_FILE = "/run/openbmc/host@%d-on";
        auto size = std::snprintf(nullptr, 0, HOST_RUNNING_FILE, 0);
        size++; // null
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, HOST_RUNNING_FILE, 0);
        std::ofstream outfile(buf.get());
        outfile.close();
        return;
    }
}

REGISTER_PROCEDURE("checkHostRunning", checkHostRunning)

} // namespace phal
} // namespace openpower
