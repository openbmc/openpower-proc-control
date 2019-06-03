#include "nmi-interface.hpp"
#include <sdbusplus/bus.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

using InternalFailure = sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;

int main(int argc, char* argv[])
{

    using namespace phosphor::logging;
    constexpr auto BUSPATH_SRESET_CONFIG = "/xyz/openbmc_project/control/host0/nmi";
    constexpr auto BUSNAME_SRESET_CONFIG = "xyz.openbmc_project.Control.Host.NMI";
    auto bus = sdbusplus::bus::new_default();
    sd_event* event = nullptr;
    auto rc = sd_event_default(&event);
    if (rc < 0)
    {
        log<level::ERR>("Error occurred during the sd_event_default",
                        entry("RC=%d", rc));
        return -1;
    }

    openpower::proc::EventPtr eventP{event};;
    event = nullptr;

    // Add sdbusplus ObjectManager
    sdbusplus::server::manager::manager objManager(bus, BUSPATH_SRESET_CONFIG);
    bus.request_name(BUSNAME_SRESET_CONFIG);

    try
    {
        openpower::proc::NMI NMI(bus,eventP,BUSPATH_SRESET_CONFIG);
        bus.attach_event(eventP.get(), SD_EVENT_PRIORITY_NORMAL);
        //phosphor::dump::core::Manager manager(eventP);

        auto rc = sd_event_loop(eventP.get());
        if (rc < 0)
        {
            log<level::ERR>("Error occurred during the sd_event_loop",
                            entry("RC=%d", rc));
        }
    }

    catch (InternalFailure& e)
    {
        return -1;
    }

#if 0
    try
    {
        openpower::proc::NMI NMI(bus,
        BUSPATH_SRESET_CONFIG);

        bus.request_name(BUSNAME_SRESET_CONFIG);
    }

    catch (const std::runtime_error& e)
    {
        log<level::ERR>("Error in trying to SRESET service. ");
    }

    while (true)
    {
        bus.process_discard();
        bus.wait();
    }

#endif

    return 0;
}
