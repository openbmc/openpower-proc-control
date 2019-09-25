extern "C" {
#include <atdb/atdb_blob.h>
}

#include "util.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <plat_trace.H>
#include <plat_utils.H>

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
            log<level::ERR>("Invalid Response from mapper",
                            entry("PATH=%s", path.c_str()),
                            entry("interface=%s", interface.c_str()));
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

int initatdb()
{
    struct atdb_blob_info* binfo;

    binfo = atdb_blob_open(ATTRIBUTES_INFO_FILE_PATH, false);
    if (!binfo)
    {
        log<level::ERR>("Unable to open atdb file",
                        entry("ATDBFILEPATH=%s", ATTRIBUTES_INFO_FILE_PATH));
        elog<InternalFailure>();
        return -1;
    }

    plat_set_atdb_context(atdb_blob_atdb(binfo));

    return 0;
}

int initTargets()
{
    log<level::INFO>("Init Targets");
    struct pdbg_target* pib;
    pdbg_for_each_class_target("pib", pib)
    {
        enum pdbg_target_status targetStatus = pdbg_target_probe(pib);

        if (targetStatus != PDBG_TARGET_ENABLED)
        {
            log<level::ERR>("Failed to init the target");
            return -1;
        }
        return 0;
    }
    return 0;
}

} // namespace util
} // namespace boot
} // namespace open_power
