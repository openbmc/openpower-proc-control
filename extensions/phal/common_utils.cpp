#include "extensions/phal/common_utils.hpp"

#include "attributes_info.H"

#include "extensions/phal/pdbg_utils.hpp"
#include "extensions/phal/phal_error.hpp"

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

    // PDBG_DTB environment variable set to CEC device tree path
    setDevtreeEnv();

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

bool isPrimaryProc(struct pdbg_target* procTarget)
{
    ATTR_PROC_MASTER_TYPE_Type type;

    // Get processor type (Primary or Secondary)
    if (DT_GET_PROP(ATTR_PROC_MASTER_TYPE, procTarget, type))
    {
        log<level::ERR>("Attribute [ATTR_PROC_MASTER_TYPE] get failed");
        throw std::runtime_error(
            "Attribute [ATTR_PROC_MASTER_TYPE] get failed");
    }

    /* Attribute value 0 corresponds to primary processor */
    if (type == ENUM_ATTR_PROC_MASTER_TYPE_ACTING_MASTER)
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
