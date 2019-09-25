extern "C" {
#include <atdb/atdb_blob.h>
}

#include "config.h"

#include "xyz/openbmc_project/Common/error.hpp"

#include <plat_trace.H>
#include <utils.H>

#include <phosphor-logging/elog-errors.hpp>
namespace openpower
{
namespace boot
{
namespace pdbg
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

void initatdb()
{
    struct atdb_blob_info* binfo;

    binfo = atdb_blob_open(ATTRIBUTES_INFO_FILE_PATH, false);
    if (!binfo)
    {
        log<level::ERR>("Unable to open atdb file",
                        entry("ATDBFILEPATH=%s", ATTRIBUTES_INFO_FILE_PATH));
        elog<InternalFailure>();
    }

    plat_set_atdb_context(atdb_blob_atdb(binfo));
}

void initTargets()
{
    log<level::INFO>("Init Targets");
    struct pdbg_target* pib;
    enum pdbg_target_status targetStatus;
    // TODO - Curently no attribute support available to check master
    // processor. First processor in the list is the master processor.
    // This should change, once master processor attribute is enabled
    pdbg_for_each_class_target("pib", pib)
    {
        if (pdbg_target_index(pib) == 0)
        {
            targetStatus = pdbg_target_probe(pib);

            if (targetStatus != PDBG_TARGET_ENABLED)
            {
                // TODO - Need handle this case
                log<level::ERR>("Failed to init the target",
                                entry("TARGETNAME=%s", pdbg_target_name(pib)));
            }
            break;
        }
    }
}

} // namespace pdbg
} // namespace boot
} // namespace openpower
