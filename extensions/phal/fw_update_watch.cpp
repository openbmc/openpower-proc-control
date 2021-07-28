#include "config.h"

#include "fw_update_watch.hpp"

#include "common_utils.hpp"
#include "create_pel.hpp"
#include "pdbg_utils.hpp"

#include <fmt/format.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

#include <filesystem>

namespace openpower
{
namespace phal
{
namespace fwupdate
{
using namespace phosphor::logging;
namespace fs = std::filesystem;

using InternalFailure =
    sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;

using Message = std::string;
using Attributes = std::variant<Message>;
using AttributeName = std::string;
using AttributeMap = std::map<AttributeName, Attributes>;
using PropertyName = std::string;
using PropertyMap = std::map<PropertyName, AttributeMap>;

void Watch::addCallback(sdbusplus::message::message& msg)
{
    //  DBusInterfaceAdded interfaces;
    sdbusplus::message::object_path objectPath;
    PropertyMap propertyMap;

    try
    {
        msg.read(objectPath, propertyMap);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>(fmt::format("Failed to parse software add signal"
                                    "Exception [{}] REPLY_SIG [{}]",
                                    e.what(), msg.get_signature())
                            .c_str());
        return;
    }

    auto iter = propertyMap.find("xyz.openbmc_project.Software.Activation");
    if (iter == propertyMap.end())
    {
        // Skip not related Software Activation
        return;
    }

    auto attr = iter->second.find("Activation");
    if (attr == iter->second.end())
    {
        // Skip not related to Activation property.
        return;
    }

    auto& imageProperty = std::get<PropertyName>(attr->second);
    if (imageProperty.empty())
    {
        // Skip, no image property
        return;
    }

    if (imageProperty ==
            "xyz.openbmc_project.Software.Activation.Activations.Ready" &&
        !isSoftwareUpdateInProgress())
    {
        log<level::INFO>("Software path interface add signal received");

        // Set status to code update in progress.
        // Noticed interface added signal triggerd twice in code update path
        // Device tree data collection is required only for the first trigger
        setSoftwareUpdateProgress(true);

        try
        {
            // Colect device tree data
            openpower::phal::fwupdate::exportDevtree();
        }
        catch (fs::filesystem_error& e)
        {
            log<level::ERR>(
                fmt::format("Filesystem error reported Error:({})", e.what())
                    .c_str());
            elog<InternalFailure>();
        }

        log<level::INFO>("Successfully exported devtree attribute data");
    }

    return;
}

void exportDevtree()
{
    // Check devtree export filter file is present
    auto path = fs::path(DEVTREE_EXPORT_FILTER_FILE);
    if (!fs::exists(path))
    {
        log<level::ERR>(
            fmt::format("devtree export filter file is not available: ({})",
                        DEVTREE_EXPORT_FILTER_FILE)
                .c_str());
        openpower::pel::createPEL("org.open_power.PHAL.Error.devtreeBackup");
        return;
    }

    // delete export data file if present
    auto expFile = fs::path(DEVTREE_EXP_FILE);
    if (fs::exists(expFile))
    {
        // delete all the files the directory
        fs::remove_all(expFile);
    }
    else
    {
        // create directory
        fs::create_directory(expFile.parent_path());
    }

    // Update PDBG_DTB value
    openpower::phal::setDevtreeEnv();

    int status = 0;
    pid_t pid = fork();
    if (pid == 0)
    {
        std::string cmd("/usr/bin/attributes ");
        cmd += "export ";
        cmd += DEVTREE_EXPORT_FILTER_FILE;
        cmd += " > ";
        cmd += DEVTREE_EXP_FILE;
        execl("/bin/sh", "sh", "-c", cmd.c_str(), 0);

        auto error = errno;
        log<level::ERR>(fmt::format("Error occurred during attributes function "
                                    "execution, errno({})",
                                    error)
                            .c_str());

        // static constexpr auto devtreeBackupMessage =
        // "org.open_power.PHAL.Error.devtreeBackup";
        openpower::pel::createPEL("org.open_power.PHAL.Error.devtreeBackup");
    }
    else if (pid > 0)
    {
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status))
        {
            log<level::ERR>("Failed to collect attribute export data");
            openpower::pel::createPEL(
                "org.open_power.PHAL.Error.devtreeBackup");
        }
    }
    else
    {
        log<level::ERR>("exportDevtree fork() failed.");
        elog<InternalFailure>();
    }
}

} // namespace fwupdate
} // namespace phal
} // namespace openpower
