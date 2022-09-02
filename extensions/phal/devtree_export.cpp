#include "fw_update_watch.hpp"

#include <fmt/format.h>

#include <phosphor-logging/elog-errors.hpp>

int main()
{
    auto eventRet = 0;

    try
    {
        auto bus = sdbusplus::bus::new_default();

        // create watch for interface added in software update.
        openpower::phal::fwupdate::Watch eWatch(bus);

        // Watch for software update
        bus.process_loop();
    }
    catch (const std::exception& e)
    {
        using namespace phosphor::logging;
        log<level::ERR>(
            fmt::format("Exception reported: [{}]", e.what()).c_str());
    }

    return eventRet;
}
