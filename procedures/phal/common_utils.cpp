extern "C"
{
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"
#include "procedures/phal/common_utils.hpp"
#include "registration.hpp"

#include <libekb.H>
#include <libipl.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

void phal_init(enum ipl_mode mode)
{
    // add callback methods for debug traces and for boot failures
    openpower::pel::addBootErrorCallbacks();

    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("pdbg target initialization failed");
    }
    // To clear trace if success
    openpower::pel::detail::processBootErrorCallback(true);

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("libekb initialization failed");
    }
    // To clear trace if success
    openpower::pel::detail::processBootErrorCallback(true);

    if (ipl_init(mode) != 0)
    {
        log<level::ERR>("ipl_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("Boot initialization failed");
    }
}

} // namespace phal
} // namespace openpower
