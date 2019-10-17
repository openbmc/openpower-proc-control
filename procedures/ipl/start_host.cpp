extern "C" {
#include <libipl.h>
}

#include "xyz/openbmc_project/Common/error.hpp"

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>
namespace openpower
{
namespace ipl
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

/**
 * @brief Starts the self boot engine on p10 position 0 to kick off a boot.
 * @return void
 */
void startHost()
{
    if (ipl_init() != 0)
    {
        log<level::ERR>("ipl_init failed to load atdb");
        elog<InternalFailure>();
    }

    if (ipl_run_major(0) > 0)
    {
        log<level::ERR>("ipl_run_major failed to start the host");
        elog<InternalFailure>();
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace ipl
} // namespace openpower
