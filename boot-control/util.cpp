#include "util.hpp"

#include "xyz/openbmc_project/Common/error.hpp"

#include <phosphor-logging/elog-errors.hpp>

#include "../config.h"
namespace open_power
{
namespace boot
{
namespace util
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

std::string getService(sdbusplus::bus::bus& bus, const std::string& path,
                       const std::string& interface)
{
    auto mapperCall = bus.new_method_call(MAPPER_BUSNAME, MAPPER_PATH,
                                          MAPPER_INTERFACE, "GetObject");
    mapperCall.append(path, std::vector<std::string>({interface}));
    std::map<std::string, std::vector<std::string>> mapperResponse;
    try
    {
        auto mapperResponseMsg = bus.call(mapperCall);
        mapperResponseMsg.read(mapperResponse);
        if (mapperResponse.empty())
        {
            log<level::ERR>("Invalid Response from mapper");
            elog<InternalFailure>();
        }
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Error in Mapper call");
        elog<InternalFailure>();
    }

    return mapperResponse.begin()->first;

}

int chassisPowerOn()
{
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    try
    {
        auto method = bus.new_method_call(SYSTEMD_SERVICE, SYSTEMD_OBJ_PATH,
                                          SYSTEMD_INTERFACE, "StartUnit");
        method.append(CHASSIS_ON_TGT);
        method.append("replace");
        bus.call_noreply(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Error in Chassis Power On");
        elog<InternalFailure>();
    }
    return 0;
}

bool isChassisOn()
{
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    auto service = getService(bus, CHASSIS_STATE_PATH, CHASSIS_STATE_OBJ);

    auto method = bus.new_method_call(service.c_str(), CHASSIS_STATE_PATH,
                                      SYSTEMD_PROPERTY_INTERFACE, "Get");
    method.append(CHASSIS_STATE_OBJ, "CurrentPowerState");

    sdbusplus::message::variant<std::string> currentChassisState;

    try
    {
        auto response = bus.call(method);
        response.read(currentChassisState);
        auto strParam = sdbusplus::message::variant_ns::get<std::string>(
            currentChassisState);
        return (strParam != CHASSIS_STATE_OFF);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Error in fetching current Chassis State");
        elog<InternalFailure>();
    }
}

} // namespace util
} // namespace boot
} // namespace open_power
