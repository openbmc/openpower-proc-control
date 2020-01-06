#include "create_pel.hpp"

#include <hwp_pel_data.H>
#include <platHwpErrParser.H>
#include <platHwpErrParserFFDC.H>
#include <unistd.h>

#include <map>
#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>
namespace openpower
{
namespace pel
{
namespace detail
{
constexpr auto mapperBusBame = "xyz.openbmc_project.ObjectMapper";
constexpr auto mapperObjectPath = "/xyz/openbmc_project/object_mapper";
constexpr auto mapperInterface = "xyz.openbmc_project.ObjectMapper";
constexpr auto loggingObjectPath = "/xyz/openbmc_project/logging";
constexpr auto loggingInterface = "xyz.openbmc_project.Logging.Create";
constexpr auto hwpMessageRegistry =
    "xyz.openbmc_project.Common.Error.HardwareProcedure";

std::string getService(sdbusplus::bus::bus& bus, const std::string& objectPath,
                       const std::string& interface)
{
    auto method = bus.new_method_call(mapperBusBame, mapperObjectPath,
                                      mapperInterface, "GetObject");
    method.append(objectPath, std::vector<std::string>({interface}));
    auto reply = bus.call(method);
    if (reply.is_method_error())
    {
        throw std::runtime_error("ObjectMapper GetObject failed");
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> response;
    reply.read(response);
    if (response.empty())
    {
        throw std::runtime_error("ObjectMapper GetObject bad response");
    }
    return response.begin()->first;
}
} // namespace detail

void createHwpPel(const fapi2::ReturnCode& rc)
{
    if (rc == fapi2::FAPI2_RC_SUCCESS)
    {
        return;
    }
    std::map<std::string, std::string> additionalData;

    fapi2::PELData rcData = fapi2::parseHwpRc(rc);
    additionalData.insert(rcData.begin(), rcData.end());

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

    auto service = detail::getService(bus, detail::loggingObjectPath,
                                      detail::loggingInterface);
    auto method =
        bus.new_method_call(service.c_str(), detail::loggingObjectPath,
                            detail::loggingInterface, "Create");
    auto level =
        sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
            sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                Error);
    method.append(detail::hwpMessageRegistry, level, additionalData);
    auto resp = bus.call(method);
    if (resp.is_method_error())
    {
        throw std::runtime_error("Error in invoking logging create method");
    }
}
} // namespace pel
} // namespace openpower
