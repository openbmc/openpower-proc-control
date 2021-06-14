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

/**
 *  @brief  Check if master processor or not
 *
 *  @return True/False
 */
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

} // namespace phal
} // namespace openpower
