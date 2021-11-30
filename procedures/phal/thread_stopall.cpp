#include "registration.hpp"

#include <attributes_info.H>
#include <fmt/format.h>
#include <libphal.H>

extern "C"
{
#include <libpdbg.h>
}

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

/**
 * @brief Stop instruction executions on all functional core's threads
 *        in the system.
 *        This procedure is used to stop all threads in the system in
 *        Attempt best case approch. Like issue core level stopall
 *        chip-op with ignore hardware error mode. Since this function
 *        is used in power-off/error path, ignore the internal error now.
 */
void threadStopAll(void)
{
    try
    {
        // initialize the pdbg.
        openpower::phal::pdbg::init();

        struct pdbg_target *procTarget, *coreTarget;
        ATTR_HWAS_STATE_Type hwasState;
        pdbg_for_each_class_target("proc", procTarget)
        {
            if (DT_GET_PROP(ATTR_HWAS_STATE, procTarget, hwasState))
            {
                log<level::ERR>(
                    fmt::format("({})Could not read HWAS_STATE attribute",
                                pdbg_target_path(procTarget))
                        .c_str());
                continue;
            }
            if (!hwasState.functional)
            {
                continue;
            }
            try
            {
                // Validate SBE state is good for chip-ops.
                openpower::phal::sbe::validateSBEState(procTarget);
            }
            catch (const std::exception& e)
            {
                // SBE is not in the valid state forchip-ops,
                // Skip the the thread stop on this processor.
                log<level::INFO>(
                    fmt::format("Skipping thread stopall on ({}), reason({})",
                                pdbg_target_path(procTarget), e.what())
                        .c_str());
                continue;
            }
            pdbg_for_each_target("core", procTarget, coreTarget)
            {
                // Issue thread stopall on functional cores.
                // Ignore error to attempt stop on all possible threads.
                if (DT_GET_PROP(ATTR_HWAS_STATE, coreTarget, hwasState))
                {
                    log<level::ERR>(
                        fmt::format("({})Could not read HWAS_STATE attribute",
                                    pdbg_target_path(coreTarget))
                            .c_str());
                    continue;
                }
                if (!hwasState.functional)
                {
                    continue;
                }
                // probe core to initiate thread stop
                if (PDBG_TARGET_ENABLED == pdbg_target_probe(coreTarget))
                {
                    // core stop with Ignore hardware errors, attempt best case
                    auto ret = core_stop(coreTarget, true);
                    if (ret)
                    {
                        log<level::WARNING>(
                            fmt::format("core_stop failed on ({}) with rc ({})",
                                        pdbg_target_path(coreTarget), ret)
                                .c_str());
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        log<level::ERR>(
            fmt::format("Thread stopall failed with exception ({})", ex.what())
                .c_str());
        // Dont throw exception on failure because, we need to proceed
        // further eventhough there is failure for proc-pre-poweroff
        return;
    }
}

REGISTER_PROCEDURE("threadStopAll", threadStopAll)

} // namespace phal
} // namespace openpower
