#include "util.hpp"

#include <fmt/format.h>

#include <phosphor-logging/elog.hpp>

#include <vector>

namespace openpower
{
namespace util
{
using namespace phosphor::logging;

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
    catch (const sdbusplus::exception::exception& e)
    {
        log<level::ERR>(fmt::format("D-Bus call exception OBJPATH={}"
                                    "INTERFACE={}  EXCEPTION={}",
                                    mapperObjectPath, mapperInterface, e.what())
                            .c_str());

        throw std::runtime_error("Service name is not found");
    }

    if (response.empty())
    {
        throw std::runtime_error("Service name response is empty");
    }
    return response.begin()->first;
}

bool isHostPoweringOff()
{
    try
    {
        constexpr auto object = "/xyz/openbmc_project/state/host0";
        constexpr auto service = "xyz.openbmc_project.State.Host";
        constexpr auto interface = "xyz.openbmc_project.State.Host";
        constexpr auto property = "CurrentHostState";
        auto bus = sdbusplus::bus::new_default();

        std::variant<std::string> retval;
        auto properties = bus.new_method_call(
            service, object, "org.freedesktop.DBus.Properties", "Get");
        properties.append(interface);
        properties.append(property);
        auto result = bus.call(properties);
        result.read(retval);

        const std::string* state = std::get_if<std::string>(&retval);
        if (state == nullptr)
        {
            std::string err = fmt::format(
                "CurrentHostState property is not set ({})", object);
            log<level::ERR>(err.c_str());
            return false;
        }
        if ((*state == "xyz.openbmc_project.State.Host.HostState."
                       "TransitioningToOff") ||
            (*state == "xyz.openbmc_project.State.Host.HostState."
                       "Off") ||
            (*state == "xyz.openbmc_project.State.Host.HostState."
                       "Quiesced"))
        {
            return true;
        }
    }
    catch (const std::exception& ex)
    {
        log<level::ERR>(
            fmt::format("Failed to read CurrentHostState property ({})",
                        ex.what())
                .c_str());
    }
    return false;
}
} // namespace util
} // namespace openpower
