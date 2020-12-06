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

    try
    {
        phal_init();
    }
    catch (const std::exception& ex)
    {
        log<level::ERR>("phal initialization failed");
    }

    rc = ipl_pre_poweroff();
    if (rc)
    {
        log<level::ERR>("pre_poweroff failed");
    }
}

REGISTER_PROCEDURE("prePoweroff", prePoweroff)

} // namespace phal
} // namespace openpower
