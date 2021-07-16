#include "fw_update_watch.hpp"

#include <fmt/format.h>

#include <phosphor-logging/elog.hpp>
#include <sdbusplus/exception.hpp>

namespace openpower
{
namespace phal
{
namespace fwupdate
{
using namespace phosphor::logging;
using Message = std::string;
using Attributes = std::variant<Message>;
using AttributeName = std::string;
using AttributeMap = std::map<AttributeName, Attributes>;
using PropertyName = std::string;
using PropertyMap = std::map<PropertyName, AttributeMap>;

void Watch::addCallback(sdbusplus::message::message& msg)
{
    log<level::INFO>("Software path interface add signal received");

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
        // Skip Not related to Activation property.
        return;
    }

    auto& imageProperty = std::get<PropertyName>(attr->second);
    if (imageProperty.empty())
    {
        // Skip, No image property
        return;
    }

    if (imageProperty ==
            "xyz.openbmc_project.Software.Activation.Activations.Ready" &&
        !isSoftwareUpdateInProgress())
    {
        // Set status to code update in progress.
        // Noticed interface added signal triggerd twice in code update path
        // Device tree data collection is required only for the first trigger
        setSoftwareUpdateProgress(true);

        // Colect device tree data
        exportDevtree();
    }

    return;
}

void Watch::exportDevtree()
{}

} // namespace fwupdate
} // namespace phal
} // namespace openpower
