#include "create_pel.hpp"

#include <libekb.H>
#include <unistd.h>

#include <map>
#include <phosphor-logging/elog.hpp>
#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>
namespace openpower
{
namespace pel
{
using namespace phosphor::logging;
namespace detail
{
static constexpr auto loggingObjectPath = "/xyz/openbmc_project/logging";
static constexpr auto loggingInterface = "xyz.openbmc_project.Logging.Create";

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
    return response.begin()->first;
}
} // namespace detail

void createBootInitErrorPEL(const FFDCData& ffdcData)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }
    try
    {
        static constexpr auto bootInitErrorMessage =
            "org.open_power.PHAL.Error.BootInit";

        std::string service = detail::getService(bus, detail::loggingObjectPath,
                                                 detail::loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), detail::loggingObjectPath,
                                detail::loggingInterface, "Create");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(bootInitErrorMessage, level, additionalData);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call exception",
                        entry("OBJPATH=%s", detail::loggingObjectPath),
                        entry("INTERFACE=%s", detail::loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Error in invoking D-Bus create interface");
    }
}

void createHWPErrorPEL(const FFDCData& ffdcData)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }
    try
    {
        static constexpr auto hwpErrorMessage = "org.open_power.PHAL.Error.HWP";
        std::string service = detail::getService(bus, detail::loggingObjectPath,
                                                 detail::loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), detail::loggingObjectPath,
                                detail::loggingInterface, "Create");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(hwpErrorMessage, level, additionalData);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call exception",
                        entry("OBJPATH=%s", detail::loggingObjectPath),
                        entry("INTERFACE=%s", detail::loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Error in invoking D-Bus create interface");
    }
}
} // namespace pel
} // namespace openpower
