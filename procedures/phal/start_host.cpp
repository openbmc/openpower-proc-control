#include <libipl.H>
#include <return_code.H>

#include <phalerror/create_pel.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

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
        openpower::pel::createBootInitErrorPEL();
        throw std::runtime_error("Boot initialization failed");
    }

    fapi2::ReturnCode rc = ipl_run_major(0);
    if (rc != fapi2::FAPI2_RC_SUCCESS &&
        rc != fapi2::FAPI2_RC_PHAL_NOT_SUPPORTED)
    {
        log<level::ERR>("step 0 failed to start the host");
        openpower::pel::createFAPIReturnCodeErrorPEL(rc);
        throw std::runtime_error("Failed to execute host start boot step");
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace phal
} // namespace openpower
