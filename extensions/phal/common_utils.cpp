#include "extensions/phal/common_utils.hpp"

#include "attributes_info.H"

#include "extensions/phal/pdbg_utils.hpp"
#include "extensions/phal/phal_env.hpp"
#include "extensions/phal/phal_error.hpp"

#include <fmt/format.h>
#include <libekb.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

/**
 * @brief Helper function to init the libekb
 *
 * @return NULL on success
 *         Throws an exception on failure
 */
static void init_libekb()
{
    // Set the log level and callback to get the traces
    libekb_set_loglevel(
        phal::env::getLogLevelFromEnv("LIBEKB_LOG", LIBEKB_LOG_IMP));
    libekb_set_logfunc(pel::detail::processLogTraceCallback, NULL);

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        throw std::runtime_error("libekb initialization failed");
    }
}

/**
 * @brief Helper function to init the libipl
 *
 * @param[in] iplType - the IPL type
 * @param[in] iplErrCallback - the callback to handle IPL error
 *
 * @return NULL on success
 *         Throws an exception on failure
 *
 * @note Refer https://github.com/open-power/ipl/blob/main/libipl/libipl.H
 *       to get more details about the supported ipl mode and type
 */
static void init_libipl(const enum ipl_type& iplType,
                        const ipl_error_callback_func_t iplErrCallback)
{
    // Set the log level and callback to get the traces
    ipl_set_loglevel(phal::env::getLogLevelFromEnv("IPL_LOG", IPL_INFO));
    ipl_set_logfunc(pel::detail::processLogTraceCallback, NULL);

    // Set the callback to handle libipl failures
    ipl_set_error_callback_func(iplErrCallback);

    ipl_set_type(iplType);

    // phal specific openpower-proc-control commands will always run
    // in the autoboot mode
    if (ipl_init(IPL_AUTOBOOT) != 0)
    {
        log<level::ERR>("ipl_init failed");
        throw std::runtime_error("libipl initialization failed");
    }
}

void phal_init(const enum ipl_type& iplType,
               const ipl_error_callback_func_t iplErrCallback)
{
    /**
     * PHAL contained many libraries, and those libraries need to
     * initialize in the below order to use libipl.
     *
     * Dependency order: libipl -> libekb -> libpdbg
     */
    init_libpdbg();
    init_libekb();
    init_libipl(iplType, iplErrCallback);
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
