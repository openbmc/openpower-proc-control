extern "C" {
#include <libpdbg.h>
}
#include "xyz/openbmc_project/Common/error.hpp"

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>
#undef VERSION
#include <p10_start_cbs.H>
namespace openpower
{
namespace p10
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

void startSbe()
{
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    int rc = -1;
    struct pdbg_target* pib;

    // TODO - Curently no attribute support available to check master
    // processor. First processor in the list is the master processor.
    // This should change, once master processor attribute is enabled
    pdbg_for_each_class_target("pib", pib)
    {
        if (pdbg_target_index(pib) == 0)
        {
            if ((rc = p10_start_cbs(pib, true)) != 0)
            {
                log<level::ERR>("p10_start_cbs failed",
                                entry("ERRORCODE=%d", rc),
                                entry("TARGETNAME=%s", pdbg_target_name(pib)));
                elog<InternalFailure>();
            }
            break;
        }
    }
}

/**
 * @brief Starts the self boot engine on p10 position 0 to kick off a boot.
 * @return void
 */
void startHost()
{
    openpower::pdbg::initatdb();
    openpower::pdbg::initTargets();
    startSbe();
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace p10
} // namespace openpower
