#include "phalerror/phal_error.hpp"
#include "procedures/phal/common_utils.hpp"
#include "registration.hpp"

#include <libekb.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

void prePoweroff(void)
{
    int rc;

    phal_init();

    rc = ipl_pre_poweroff();
    if (rc)
    {
        log<level::ERR>("pre_poweroff failed",
                        entry("Failed for %d procs", rc));
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("pre_poweroff failed");
    }
}

REGISTER_PROCEDURE("prePoweroff", prePoweroff)

} // namespace phal
} // namespace openpower
