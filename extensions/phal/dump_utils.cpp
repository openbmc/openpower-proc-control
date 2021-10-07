#include "dump_utils.hpp"

#include "util.hpp"

#include <fmt/format.h>

#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/exception.hpp>
#include <sdbusplus/server.hpp>

namespace openpower::phal::dump
{

using namespace phosphor::logging;

/**
 *  Callback for dump request properties change signal monitor
 *
 * @param[in] msg         Dbus message from the dbus match infrastructure
 * @param[in] path        The object path we are monitoring
 * @param[out] inProgress Used to break out of our dbus wait loop
 * @reutn Always non-zero indicating no error, no cascading callbacks
 */
uint32_t dumpStatusChanged(sdbusplus::message::message& msg, std::string path,
                           bool& inProgress)
{
    // reply (msg) will be a property change message
    std::string interface;
    std::map<std::string, std::variant<std::string, uint8_t>> property;
    msg.read(interface, property);

    // looking for property Status changes
    std::string propertyType = "Status";
    auto dumpStatus = property.find(propertyType);

    if (dumpStatus != property.end())
    {
        const std::string* status =
            std::get_if<std::string>(&(dumpStatus->second));

        if ((nullptr != status) && ("xyz.openbmc_project.Common.Progress."
                                    "OperationStatus.InProgress" != *status))
        {
            // dump is done, trace some info and change in progress flag
            log<level::INFO>(
                fmt::format("Dump created : path={}", path.c_str()).c_str());
            inProgress = false;
        }
    }

    return 1; // non-negative return code for successful callback
}

/**
 * Register a callback for dump progress status changes
 *
 * @param[in] path The object path of the dump to monitor
 */
void monitorDump(const std::string& path)
{
    bool inProgress = true; // callback will update this

    // setup the signal match rules and callback
    std::string matchInterface = "xyz.openbmc_project.Common.Progress";
    auto bus = sdbusplus::bus::new_system();

    std::unique_ptr<sdbusplus::bus::match_t> match =
        std::make_unique<sdbusplus::bus::match_t>(
            bus,
            sdbusplus::bus::match::rules::propertiesChanged(
                path.c_str(), matchInterface.c_str()),
            [&](auto& msg) {
                return dumpStatusChanged(msg, path, inProgress);
            });

    // wait for dump status to be completed (complete == true)
    log<level::INFO>("dump requested (waiting)");
    while (true == inProgress)
    {
        bus.wait(0);
        bus.process_discard();
    }
    log<level::INFO>("dump completed");
}

void requestDump(const DumpParameters& dumpParameters)
{
    log<level::INFO>(fmt::format("Requesting SBE Dump PEL({}) Index({})",
                                 dumpParameters.logId, dumpParameters.unitId)
                         .c_str());

    constexpr auto path = "/org/openpower/dump";
    constexpr auto interface = "xyz.openbmc_project.Dump.Create";
    constexpr auto function = "CreateDump";

    sdbusplus::message::message method;

    auto bus = sdbusplus::bus::new_default();

    try
    {
        std::string service = util::getService(bus, path, interface);
        auto method =
            bus.new_method_call(service.c_str(), path, interface, function);

        // dbus call arguments
        std::map<std::string, std::variant<std::string, uint64_t>> createParams;
        createParams["com.ibm.Dump.Create.CreateParameters.ErrorLogId"] =
            uint64_t(dumpParameters.logId);
        if (DumpType::SBE == dumpParameters.dumpType)
        {
            createParams["com.ibm.Dump.Create.CreateParameters.DumpType"] =
                "com.ibm.Dump.Create.DumpType.SBE";
            createParams["com.ibm.Dump.Create.CreateParameters.FailingUnitId"] =
                dumpParameters.unitId;
        }
        method.append(createParams);

        auto response = bus.call(method);

        // reply will be type dbus::ObjectPath
        sdbusplus::message::object_path reply;
        response.read(reply);

        // monitor dump progress
        monitorDump(reply);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>(fmt::format("D-Bus call createDump exception",
                                    "OBJPATH={}, INTERFACE={}, EXCEPTION={}",
                                    path, interface, e.what())
                            .c_str());
        throw std::runtime_error(
            "Error in invoking D-Bus createDump interface");
    }
    catch (std::exception& e)
    {
        throw e;
    }
}

} // namespace openpower::phal::dump
