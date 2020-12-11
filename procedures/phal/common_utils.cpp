extern "C"
{
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"
#include "procedures/phal/common_utils.hpp"

#include <libekb.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

/**
 * @brief This function is used to get log level from given environment
 *        variable which is set else used passed default level
 *
 * @param[in] env used to pass environment variable name
 * @param[in] dValue used to pass default log level
 *
 * @return log level if set in environment else given default level
 *
 */
static inline uint8_t getLogLevelFromEnv(const char* env, const uint8_t dValue)
{
    auto logLevel = dValue;
    try
    {
        if (const char* env_p = std::getenv(env))
        {
            logLevel = std::stoi(env_p);
        }
    }
    catch (std::exception& e)
    {
        log<level::ERR>(("Conversion Failure"), entry("ENVIRONMENT=%s", env),
                        entry("EXCEPTION=%s", e.what()));
    }
    return logLevel;
}

/**
 * @brief Helper function to hanlde pdbg trace since prototype is
 *        different to use common op-proc log process function
 *
 * @param[in] int used to pass log level but not using
 * @param[in] fmt used to pass format to print as trace
 * @param[in] ap used to pass arguments for trace
 *
 * @return NULL
 *
 */
static void pDBGLogTraceCallbackHelper(int, const char* fmt, va_list ap)
{
    openpower::pel::detail::processLogTraceCallback(NULL, fmt, ap);
}

void initPHAL(const ipl_error_callback_func_t iplErrCallbackFunc,
              const enum ipl_mode iplMode, const enum ipl_type iplType)
{
    // Get individual phal repos log level from environment variable
    // and update the  log level.
    pdbg_set_loglevel(getLogLevelFromEnv("PDBG_LOG", PDBG_INFO));
    libekb_set_loglevel(getLogLevelFromEnv("LIBEKB_LOG", LIBEKB_LOG_IMP));
    ipl_set_loglevel(getLogLevelFromEnv("IPL_LOG", IPL_INFO));

    // add callback for debug traces
    pdbg_set_logfunc(pDBGLogTraceCallbackHelper);
    libekb_set_logfunc(openpower::pel::detail::processLogTraceCallback, NULL);
    ipl_set_logfunc(openpower::pel::detail::processLogTraceCallback, NULL);

    // add callback for ipl failures
    ipl_set_error_callback_func(iplErrCallbackFunc);

    // setting expected ipl type
    ipl_set_type(iplType);

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

    if (ipl_init(iplMode) != 0)
    {
        log<level::ERR>("ipl_init failed");
        throw std::runtime_error("libipl initialization failed");
    }
}

} // namespace phal
} // namespace openpower
