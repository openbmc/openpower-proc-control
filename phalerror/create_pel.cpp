#include "create_pel.hpp"

#include <libekb.H>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <map>
#include <phosphor-logging/elog.hpp>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include <xyz/openbmc_project/Logging/Create/server.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>

namespace openpower
{
using namespace phosphor::logging;

namespace util
{
std::string getService(sdbusplus::bus::bus& bus, const std::string& objectPath,
                       const std::string& interface)
{
    constexpr auto mapperBusBame = "xyz.openbmc_project.ObjectMapper";
    constexpr auto mapperObjectPath = "/xyz/openbmc_project/object_mapper";
    constexpr auto mapperInterface = "xyz.openbmc_project.ObjectMapper";
    std::vector<std::pair<std::string, std::vector<std::string>>> response;
    auto method = bus.new_method_call(mapperBusBame, mapperObjectPath,
                                      mapperInterface, "GetObject");
    method.append(objectPath, std::vector<std::string>({interface}));
    try
    {
        auto reply = bus.call(method);
        reply.read(response);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call exception",
                        entry("OBJPATH=%s", mapperObjectPath),
                        entry("INTERFACE=%s", mapperInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Service name is not found");
    }

    if (response.empty())
    {
        throw std::runtime_error("Service name response is empty");
    }
    return response.begin()->first;
}
} // namespace util

namespace pel
{
void createBootErrorPEL(const FFDCData& ffdcData, const json& calloutData)
{
    constexpr auto loggingObjectPath = "/xyz/openbmc_project/logging";
    constexpr auto loggingInterface = "xyz.openbmc_project.Logging.Create";

    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }

    auto calloutDataStr = calloutData.dump();
    std::string calloutFilename("/tmp/phalPELCallouts.json");

    // Using C api for file operation to get file descriptor which
    // is expecting by CreateWithFFDCFiles dbus api
    FILE* jsonCalloutsFile = std::fopen(calloutFilename.c_str(), "w+");

    if (jsonCalloutsFile == nullptr)
    {
        log<level::ERR>("Failed to create phalPELCallouts file",
                        entry("FILENAME=%s", calloutFilename.c_str()),
                        entry("ERRNO=%d", errno),
                        entry("ERRMSG=%s", strerror(errno)));
        throw std::runtime_error("Failed to create phalPELCallouts file");
    }

    unsigned int rc = std::fwrite(calloutDataStr.c_str(), 1,
                                  calloutDataStr.size(), jsonCalloutsFile);

    if (std::ferror(jsonCalloutsFile))
    {
        log<level::ERR>("Failed to write phal PEL Callout info",
                        entry("FILENAME=%s", calloutFilename.c_str()),
                        entry("ERRNO=%d", errno),
                        entry("ERRMSG=%s", strerror(errno)));
        throw std::runtime_error("Failed to write phalPELCallouts info");
    }
    else if (rc != calloutDataStr.size())
    {
        log<level::WARNING>("Could not write all phal callout info",
                            entry("FILENAME=%s", calloutFilename.c_str()),
                            entry("WRITTEN_BYTE=%d", rc),
                            entry("TOTAL_BYTE=%d", calloutDataStr.size()));
    }

    // Setting file seek position to begining to consume by PEL
    std::fseek(jsonCalloutsFile, 0, SEEK_SET);
    if (std::ferror(jsonCalloutsFile))
    {
        log<level::ERR>("Failed to set SEEK_SET for phalPELCallouts file",
                        entry("FILENAME=%s", calloutFilename.c_str()),
                        entry("ERRNO=%d", errno),
                        entry("ERRMSG=%s", strerror(errno)));
    }

    sdbusplus::message::unix_fd jsonFd = fileno(jsonCalloutsFile);
    if (jsonFd == -1)
    {
        log<level::ERR>("Failed to get FD for phalPELCallouts file",
                        entry("FILENAME=%s", calloutFilename.c_str()),
                        entry("ERRNO=%d", errno),
                        entry("ERRMSG=%s", strerror(errno)));
    }

    std::vector<std::tuple<
        sdbusplus::xyz::openbmc_project::Logging::server::Create::FFDCFormat,
        uint8_t, uint8_t, sdbusplus::message::unix_fd>>
        pelCalloutInfo;

    pelCalloutInfo.push_back(std::make_tuple(
        sdbusplus::xyz::openbmc_project::Logging::server::Create::FFDCFormat::
            JSON,
        static_cast<uint8_t>(0xCA), static_cast<uint8_t>(0x01), jsonFd));

    try
    {
        static constexpr auto bootErrorMessage =
            "org.open_power.PHAL.Error.Boot";
        std::string service =
            util::getService(bus, loggingObjectPath, loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), loggingObjectPath,
                                loggingInterface, "CreateWithFFDCFiles");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(bootErrorMessage, level, additionalData, pelCalloutInfo);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call exception",
                        entry("OBJPATH=%s", loggingObjectPath),
                        entry("INTERFACE=%s", loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        std::fclose(jsonCalloutsFile);
        std::remove(calloutFilename.c_str());
        throw std::runtime_error(
            "Error in invoking D-Bus logging create interface");
    }

    std::fclose(jsonCalloutsFile);
    std::remove(calloutFilename.c_str());
}
} // namespace pel
} // namespace openpower
