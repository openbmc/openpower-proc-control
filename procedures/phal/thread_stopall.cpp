#include "extensions/phal/create_pel.hpp"
#include "extensions/phal/dump_utils.hpp"
#include "registration.hpp"

#include <attributes_info.H>
#include <fmt/format.h>
#include <libphal.H>
#include <phal_exception.H>
extern "C"
{
#include <libpdbg.h>
}
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{
using namespace openpower::pel;
using namespace openpower::phal;
using namespace openpower::phal::sbe;
using namespace openpower::phal::exception;
using namespace phosphor::logging;

/**
 * @brief Stop instruction executions on all functional threads in the
 *        host processors.
 *        This procedure is used to stop all threads in the system in
 *        Attempt best case approch. Like issue processor level stopall
 *        chip-op with ignore hardware error mode. Since this function
 *        is used in power-off/error path, ignore the internal error now.
 */
void threadStopAll(void)
{
    // CMD details based on SBE spec, used for logging purpose
    constexpr auto SBEFIFO_CMD_CLASS_INSTRUCTION = 0xA700;
    constexpr auto SBEFIFO_CMD_CONTROL_INSN = 0x01;
    uint32_t cmd = SBEFIFO_CMD_CLASS_INSTRUCTION | SBEFIFO_CMD_CONTROL_INSN;

    try
    {
        // initialize the pdbg.
        openpower::phal::pdbg::init();

        struct pdbg_target* procTarget;
        // struct pdbg_target* procTarget;
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
                threadStopProc(procTarget);
            }
            catch (const sbeError_t& sbeError)
            {
                if (sbeError.errType() == SBE_CHIPOP_NOT_ALLOWED)
                {
                    // SBE is not ready to accept chip-ops,
                    // Skip the request, no additional error handling required.
                    log<level::INFO>(
                        fmt::format("threadStopAll: Skipping ({}) on proc({})",
                                    sbeError.what(),
                                    pdbg_target_index(procTarget))
                            .c_str());
                    continue;
                }

                log<level::ERR>(
                    fmt::format("threadStopAll failed({}) on proc({})",
                                sbeError.what(), pdbg_target_index(procTarget))
                        .c_str());

                std::string event;
                bool dumpIsRequired = false;

                if (sbeError.errType() == SBE_CMD_TIMEOUT)
                {
                    event = "org.open_power.Processor.Error.SbeChipOpTimeout";
                    dumpIsRequired = true;
                }
                else
                {
                    event = "org.open_power.Processor.Error.SbeChipOpFailure";
                }

                uint32_t index = pdbg_target_index(procTarget);
                // To store additional data about ffdc.
                FFDCData pelAdditionalData;

                // SRC6 : [0:15] chip position
                //        [16:23] command class,  [24:31] Type
                pelAdditionalData.emplace_back(
                    "SRC6", std::to_string((index << 16) | cmd));
                auto logId =
                    createSbeErrorPEL(event, sbeError, pelAdditionalData);

                if (dumpIsRequired)
                {
                    // Request SBE Dump
                    using namespace openpower::phal::dump;
                    DumpParameters dumpParameters = {
                        logId, index, SBE_DUMP_TIMEOUT, DumpType::SBE};
                    requestDump(dumpParameters);
                }
                continue;
            }
            log<level::INFO>(
                fmt::format("Processor thread stopall completed on proc({})",
                            pdbg_target_index(procTarget))
                    .c_str());
        }
    }
    // Capture genaral exception
    catch (const std::exception& ex)
    {
        // This failure could be related to BMC firmware
        // Dont throw exception on failure because, need to proceed
        // further to complete power-off/reboot.
        log<level::ERR>(
            fmt::format("threadStopAll: Exception({})", ex.what()).c_str());

        // To store additional data about ffdc.
        FFDCData pelAdditionalData;

        // SRC6 : [0:15] chip position, setting 0xFF to indicate generic fail
        //        [16:23] command class,  [24:31] Type
        pelAdditionalData.emplace_back("SRC6",
                                       std::to_string((0xFF << 16) | cmd));
        json jsonCalloutDataList;
        jsonCalloutDataList = json::array();
        json jsonCalloutData;
        jsonCalloutData["Procedure"] = "BMC0001";
        jsonCalloutData["Priority"] = "H";
        jsonCalloutDataList.emplace_back(jsonCalloutData);
        openpower::pel::createErrorPEL(
            "org.open_power.Processor.Error.SbeChipOpFailure",
            jsonCalloutDataList, pelAdditionalData);
        return;
    }
}

REGISTER_PROCEDURE("threadStopAll", threadStopAll)

} // namespace phal
} // namespace openpower
