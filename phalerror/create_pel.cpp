#include "create_pel.hpp"

#include <hwp_pel_data.H>
#include <platHwpErrParser.H>
#include <platHwpErrParserFFDC.H>
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
        log<level::ERR>("D-Bus call expection",
                        entry("OBJPATH=%s", mapperObjectPath),
                        entry("INTERFACE=%s", mapperInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Service name is not found");
    }
    return response.begin()->first;
}
} // namespace detail

void createBootInitErrorPEL()
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    try
    {
        static constexpr auto iplBootInitMessage =
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
        method.append(iplBootInitMessage, level, additionalData);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call expection",
                        entry("OBJPATH=%s", detail::loggingObjectPath),
                        entry("INTERFACE=%s", detail::loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Error in invoking D-Bus create interface");
    }
}

void createFAPIReturnCodeErrorPEL(const fapi2::ReturnCode& rc)
{
    if (rc == fapi2::FAPI2_RC_SUCCESS)
    {
        return;
    }

    fapi2::PELData additionalData = fapi2::parseHwpRc(rc);

    // Iterate through the FFDC sections, adding each to the error log
    const fapi2::ErrorInfo* errorInfo = rc.getErrorInfo();
    for (auto error : errorInfo->iv_ffdcs)
    {
        uint32_t size = 0;
        uint32_t ffdcId = error->getFfdcId();
        auto errData = error->getData(size);
        fapi2::PELData ffdc = fapi2::parseHwpFfdc(ffdcId, errData, size);
        additionalData.insert(ffdc.begin(), ffdc.end());
    }

    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));

    try
    {
        static constexpr auto retCodeMessage =
			"org.open_power.PHAL.Error.FAPIReturnCode";
        std::string service = detail::getService(bus, detail::loggingObjectPath,
                                                 detail::loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), detail::loggingObjectPath,
                                detail::loggingInterface, "Create");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(retCodeMessage, level, additionalData);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call expection",
                        entry("OBJPATH=%s", detail::loggingObjectPath),
                        entry("INTERFACE=%s", detail::loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error("Error in invoking D-Bus create interface");
    }
}
} // namespace pel
} // namespace openpower
