extern "C" {
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"

#include <libekb.H>
#include <libipl.H>

#include <phosphor-logging/log.hpp>
#include <registration.hpp>
namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

/**
 * @brief Starts the self boot engine on POWER processor position 0
 *        to kick off a boot.
 * @return void
 */
void startHost()
{
    // add callback s for debug traces and for boot failures
    openpower::pel::addBootErrorCallbacks();

    // callback will be created upon failure which will create the PEL
    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        throw std::runtime_error("pdbg target initialization failed");
    }

    // callback will be created upon failure which will create the PEL
    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        throw std::runtime_error("libekb initialization failed");
    }

    // callback will be created upon failure which will create the PEL
    if (ipl_init(IPL_AUTOBOOT) != 0)
    {
        log<level::ERR>("ipl_init failed");
        throw std::runtime_error("Boot initialization failed");
    }

    // callback will be created upon failure which will create the PEL
    int rc = ipl_run_major(0);
    if (rc > 0)
    {
        log<level::ERR>("step 0 failed to start the host");
        throw std::runtime_error("Failed to execute host start boot step");
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace phal
} // namespace openpower
