extern "C"
{
#include <libpdbg.h>
}
#include "attributes_info.H"

#include "phalerror/phal_error.hpp"
#include "procedures/phal/common_utils.hpp"

#include <libekb.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

void phal_init(enum ipl_mode mode)
{
    // TODO: Setting boot error callback should not be in common code
    //       because, we wont get proper reason in PEL for failure.
    //       So, need to make code like caller of this function pass error
    //       handling callback.
    // add callback methods for debug traces and for boot failures
    openpower::pel::addBootErrorCallbacks();

    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        throw std::runtime_error("pdbg target initialization failed");
    }

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        throw std::runtime_error("libekb initialization failed");
    }

    if (ipl_init(mode) != 0)
    {
        log<level::ERR>("ipl_init failed");
        throw std::runtime_error("libipl initialization failed");
    }
}

bool isMasterProc(struct pdbg_target* procTarget)
{
    ATTR_PROC_MASTER_TYPE_Type type;

    // Get processor type (Master or Alt-master)
    if (DT_GET_PROP(ATTR_PROC_MASTER_TYPE, procTarget, type))
    {
        log<level::ERR>("Attribute [ATTR_PROC_MASTER_TYPE] get failed");
        throw std::runtime_error(
            "Attribute [ATTR_PROC_MASTER_TYPE] get failed");
    }

    /* Attribute value 0 corresponds to master processor */
    if (type == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

pdbg_target* getFsiTarget(struct pdbg_target* procTarget)
{
    auto procIdx = pdbg_target_index(procTarget);
    char path[16];
    sprintf(path, "/proc%d/pib", procIdx);

    pdbg_target* pibTarget = pdbg_target_from_path(nullptr, path);
    if (nullptr == pibTarget)
    {
        log<level::ERR>("pib path of target not found");
        return nullptr;
    }

    // probe PIB and ensure it's enabled
    if (PDBG_TARGET_ENABLED != pdbg_target_probe(pibTarget))
    {
        log<level::ERR>("probe on pib target failed");
        return nullptr;
    }

    // now build FSI path and read the input reg
    sprintf(path, "/proc%d/fsi", procIdx);
    pdbg_target* fsiTarget = pdbg_target_from_path(nullptr, path);
    if (nullptr == fsiTarget)
    {
        log<level::ERR>("fsi path or target not found");
        return nullptr;
    }

    return fsiTarget;
}

uint32_t getCFAM(struct pdbg_target* procTarget, const uint16_t reg,
                 uint32_t& val)
{

    pdbg_target* fsiTarget = getFsiTarget(procTarget);
    if (nullptr == fsiTarget)
    {
        log<level::ERR>("getCFAM: fsi path or target not found");
        return -1;
    }

    auto rc = fsi_read(fsiTarget, reg, &val);
    if (rc)
    {
        log<level::ERR>("failed to read input cfam", entry("RC=%u", rc),
                        entry("CFAM=0x%X", reg));
        return rc;
    }
    return 0;
}

uint32_t putCFAM(struct pdbg_target* procTarget, const uint16_t reg,
                 const uint32_t val)
{
    pdbg_target* fsiTarget = getFsiTarget(procTarget);
    if (nullptr == fsiTarget)
    {
        log<level::ERR>("putCFAM: fsi path or target not found");
        return -1;
    }

    auto rc = fsi_write(fsiTarget, reg, val);
    if (rc)
    {
        log<level::ERR>("failed to write input cfam", entry("RC=%u", rc),
                        entry("CFAM=0x%X", reg));
        return rc;
    }
    return 0;
}

} // namespace phal
} // namespace openpower
