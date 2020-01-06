#include <libipl.H>
#include <libipl_retcode.H>
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
        openpower::pel::createIplInitErrorPEL();
    }

    fapi2::ReturnCode rc = ipl_run_major_ret(0);
    if (rc != fapi2::FAPI2_RC_SUCCESS && rc != fapi2::FAPI2_RC_NOT_SUPPORTED)
    {
        log<level::ERR>("step 0 failed to start the host");
        openpower::pel::createHwpErrorPEL(rc);
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace phal
} // namespace openpower
