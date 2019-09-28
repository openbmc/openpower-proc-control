extern "C" {
#include <libpdbg.h>
}

#include "bmc_boot_steps.hpp"
#include "pdbg.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <p10_start_cbs.H>

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
namespace openpower
{
namespace boot
{
namespace bmc_steps
{
using namespace phosphor::logging;

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

void stubbedStep()
{
    log<level::INFO>("Step is a stub");
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
