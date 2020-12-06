extern "C"
{
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"
#include "registration.hpp"

#include <libekb.H>
#include <libipl.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

void phal_init()
{
    openpower::pel::addBootErrorCallbacks();

    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("pdbg_targets_init failed");
    }

    openpower::pel::detail::processBootErrorCallback(true);

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("libekb initialization failed");
    }

    openpower::pel::detail::processBootErrorCallback(true);

    if (ipl_init(IPL_AUTOBOOT) != 0)
    {
        log<level::ERR>("ipl_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("ipl_init failed");
    }
}

void prePoweroff(void)
{
    int rc;

    log<level::INFO>("Starting prePoweroff");
    phal_init();

    rc = ipl_pre_poweroff();
    if (rc)
    {
        log<level::ERR>("p10_pre_poweroff failed for proc");
        throw std::runtime_error("p10_pre_poweroff failed for proc");
    }
}

REGISTER_PROCEDURE("prePoweroff", prePoweroff)

} // namespace phal
} // namespace openpower
