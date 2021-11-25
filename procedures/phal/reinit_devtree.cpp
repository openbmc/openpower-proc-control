#include "config.h"

#include "extensions/phal/create_pel.hpp"
#include "registration.hpp"
#include "temporary_file.hpp"

#include <fcntl.h>
#include <fmt/format.h>

#include <phosphor-logging/elog-errors.hpp>

#include <cstdio>
#include <filesystem>

extern "C"
{
#include <dtree.h>
}

namespace openpower
{
namespace phal
{
/**
 * @brief reinitialize the devtree attributes.
 * In the regular host boot path devtree attribute need to
 * initialize the default data and also some of the selected
 * attributes need to preserve with previous boot value.
 * Preserve attribute list is available BMC pre-defined location.
 * This function helps to meet the host ipl requirement
 * related to attribute persistency management for host ipl.
 * Steps involved
 * 1. create attribute data file from devtree r/w version based on
 *    the attribute list file installed in bmc.
 * 2. copy devtree r/o version r/w version to initialize all attributes
 *    to default data
 * 3. apply step 1 attribute file on top of the r/w version file.
 */

void reinitDevtree()
{
    constexpr auto ERROR_DEVTREE_BACKUP =
        "org.open_power.PHAL.Error.devtreeBackup";
    constexpr auto ERROR_DEVTREE_DATASYNC =
        "org.open_power.PHAL.Error.devtreeSync";
    using namespace phosphor::logging;
    using FILE_Ptr = std::unique_ptr<FILE, decltype(&::fclose)>;
    namespace fs = std::filesystem;

    log<level::INFO>("reinitDevtree: started");

    // create temporary file to store the devtree export data
    openpower::util::TemporaryFile tmpFile{};

    // Check devtree reinit attributes list file is present
    auto attrFile = fs::path(DEVTREE_REINIT_ATTRS_LIST);
    if (!fs::exists(attrFile))
    {
        log<level::ERR>(
            fmt::format(
                "devtree attribute export list file is not available: ({})",
                DEVTREE_REINIT_ATTRS_LIST)
                .c_str());
        openpower::pel::createPEL(ERROR_DEVTREE_BACKUP);
        throw std::runtime_error("reinitDevtree: missing export list file");
    }

    // get temporary datafile pointer.
    FILE_Ptr fpExport(fopen(tmpFile.getPath().c_str(), "w+"), fclose);

    if (fpExport.get() == nullptr)
    {
        log<level::ERR>(fmt::format("Temporary data file failed to open: ({})",
                                    tmpFile.getPath().c_str())
                            .c_str());
        openpower::pel::createPEL(ERROR_DEVTREE_BACKUP);
        throw std::runtime_error(
            "reinitDevtree: failed to open temp data file");
    }

    // export devtree data based export attribute list.
    auto ret = dtree_cronus_export(CEC_DEVTREE_RW_PATH, CEC_INFODB_PATH,
                                   DEVTREE_REINIT_ATTRS_LIST, fpExport.get());
    if (ret)
    {
        log<level::ERR>(
            fmt::format("Failed({}) to collect attribute export data", ret)
                .c_str());
        openpower::pel::createPEL(ERROR_DEVTREE_BACKUP);
        throw std::runtime_error(
            "reinitDevtree: dtree_cronus_export function failed");
    }
    // Close the temporary data file.
    fpExport.reset();

    // Copy devtree r/o version to r/w version.
    try
    {
        const auto copyOptions =
            std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(CEC_DEVTREE_RO_PATH, CEC_DEVTREE_RW_PATH,
                              copyOptions);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(
            fmt::format("Failed to copy devtree file ({})", e.what()).c_str());
        throw;
    }

    // get r/o version data file pointer
    FILE_Ptr fpImport(fopen(tmpFile.getPath().c_str(), "r"), fclose);

    // Update Devtree r/w version with data file attribute data.
    ret = dtree_cronus_import(CEC_DEVTREE_RW_PATH, CEC_INFODB_PATH,
                              fpImport.get());
    if (ret)
    {
        log<level::ERR>(
            fmt::format("Failed({}) to update attribute data", ret).c_str());
        openpower::pel::createPEL(ERROR_DEVTREE_DATASYNC);
        throw std::runtime_error(
            "reinitDevtree: dtree_cronus_import function failed");
    }
}

REGISTER_PROCEDURE("reinitDevtree", reinitDevtree)

} // namespace phal
} // namespace openpower
