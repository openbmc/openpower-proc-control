extern "C" {
#include <atdb/atdb_blob.h>
}

#include "config.h"

#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <plat_trace.H>
#include <utils.H>

#include <phosphor-logging/elog-errors.hpp>
namespace openpower
{
namespace boot
{
namespace util
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
    // Init targets only if chassis is on
    if (openpower::boot::util::isChassisOn())
    {
        log<level::INFO>("Init Targets");
        struct pdbg_target* pib;
        pdbg_for_each_class_target("pib", pib)
        {
            enum pdbg_target_status targetStatus = pdbg_target_probe(pib);

            if (targetStatus != PDBG_TARGET_ENABLED)
            {
                log<level::ERR>("Failed to init the target");
                elog<InternalFailure>();
            }
            return;
        }
    }
    else
    {
        log<level::INFO>("Chassis should be in on before init targets");
    }
}

} // namespace pdbg
} // namespace util
} // namespace boot
} // namespace openpower
