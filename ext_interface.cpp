#include <string>
#include <sdbusplus/server.hpp>
#include <phosphor-logging/log.hpp>
#include <ext_interface.hpp>

// Mapper
constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
constexpr auto MAPPER_PATH = "/xyz/openbmc_project/object_mapper";
constexpr auto MAPPER_INTERFACE = "xyz.openbmc_project.ObjectMapper";

// Reboot count
constexpr auto REBOOTCOUNTER_PATH("/org/openbmc/sensors/host/BootCount");
constexpr auto REBOOTCOUNTER_INTERFACE("org.openbmc.SensorValue");

using namespace phosphor::logging;

/**
 * @brief Get DBUS service for input interface via mapper call
 *
 * This is an internal function to be used only by functions within this
 * file.
 *
 * @param[in] bus -  DBUS Bus Object
 * @param[in] intf - DBUS Interface
 * @param[in] path - DBUS Object Path
 *
 * @return distinct dbus name for input interface/path
 **/
std::string getService(sdbusplus::bus::bus& bus,
                       const std::string& intf,
                       const std::string& path)
{

    auto mapper = bus.new_method_call(MAPPER_BUSNAME,
                                      MAPPER_PATH,
                                      MAPPER_INTERFACE,
                                      "GetObject");

    mapper.append(path);
    mapper.append(std::vector<std::string>({intf}));

    auto mapperResponseMsg = bus.call(mapper);

    if (mapperResponseMsg.is_method_error())
    {
        // TODO openbmc/openbmc#851 - Once available, throw returned error
        throw std::runtime_error("ERROR in mapper call");
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);

    if (mapperResponse.begin() == mapperResponse.end())
    {
        // TODO openbmc/openbmc#851 - Once available, throw returned error
        throw std::runtime_error("ERROR in reading the mapper response");
    }

    return mapperResponse.begin()->first;
}


int getBootCount()
{
    auto bus = sdbusplus::bus::new_default();

    auto rebootSvc = getService(bus,
			                    REBOOTCOUNTER_INTERFACE,
								REBOOTCOUNTER_PATH);

    sdbusplus::message::variant<int> rebootCount = 0;
    auto method = bus.new_method_call(rebootSvc.c_str(),
                                      REBOOTCOUNTER_PATH,
                                      REBOOTCOUNTER_INTERFACE,
                                      "getValue");

    auto reply = bus.call(method);
    if (reply.is_method_error())
    {
        log<level::ERR>("Error in BOOTCOUNT getValue");
        // TODO openbmc/openbmc#851 - Once available, throw returned error
        throw std::runtime_error("ERROR in reading BOOTCOUNT");
    }
    reply.read(rebootCount);

    return (sdbusplus::message::variant_ns::get<int>(rebootCount));
}
