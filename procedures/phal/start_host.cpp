#include "xyz/openbmc_project/Common/error.hpp"

#include <libipl.H>
#include <return_code.H>

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>
namespace openpower
{
namespace phal
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

/**
 * @brief Starts the self boot engine on POWER processor position 0
 *        to kick off a boot.
 * @return void
 */
void startHost()
{
    if (ipl_init() != 0)
    {
        log<level::ERR>("ipl_init failed");
        // TODO ibm-openbmc#1470
        elog<InternalFailure>();
    }

    fapi2::ReturnCode rc = ipl_run_major(0);
    if (rc != fapi2::FAPI2_RC_SUCCESS && rc != static_cast<uint32_t>(-1))
    {
        log<level::ERR>("step 0 failed to start the host");
        throw rc;
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace phal
} // namespace openpower
