#include <cstdlib>
#include <iostream>
#include <exception>
#include <sdbusplus/bus.hpp>
#include "boot_manager.hpp"

constexpr auto BOOT_OBJPATH="/org/open_power/state/boot";
constexpr auto BOOT_BUSNAME="org.open_power.State.Boot";

int main(int argc, char *argv[])
{

    auto bus = sdbusplus::bus::new_default();

    // For now, we only have one instance of the host
    auto objPathInst = std::string{HOST_OBJPATH} + '0';

    // Add sdbusplus ObjectManager.
    sdbusplus::server::manager::manager objManager(bus, objPathInst.c_str());

    open_power::boot::Manager manager(bus, objPathInst.c_str());

    bus.request_name(HOST_BUSNAME);

    while (true)
    {
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
