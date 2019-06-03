#include "soft_reset.hpp"
#include <sdbusplus/bus.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

int main(int argc, char* argv[])
{

    constexpr auto BUSPATH_SRESET_CONFIG = "/xyz/openbmc_project/control/host0";
    constexpr auto BUSNAME_SRESET_CONFIG = "xyz.openbmc_project.Control.Host.NMI";
    auto bus = sdbusplus::bus::new_default();

    try
    {
        openpower::proc::SoftReset softReset(bus,
        BUSPATH_SRESET_CONFIG);

        bus.request_name(BUSNAME_SRESET_CONFIG);
    }

    catch (const std::runtime_error& e)
    {
         printf("Invalid argument");
    }

    while (true)
    {
        bus.process_discard();
        bus.wait();
    }

    return 0;
}
