#include "boot_control.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <CLI/CLI.hpp>
#include <iostream>
#include <optional>
#include <phosphor-logging/elog-errors.hpp>

using BootSeq = std::pair<uint8_t, uint8_t>;

BootSeq getBootSeq(std::string steps)
{
    using namespace phosphor::logging;
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    BootSeq bs;
    auto splitPos = steps.find("..");
    if (splitPos == std::string::npos)
    {
        uint8_t number = std::stoi(steps);
        bs = std::make_pair(number, number);
    }
    else
    {
        bs = std::make_pair(std::stoi(steps.substr(0, splitPos)),
                            std::stoi(steps.substr(splitPos + 2)));
    }
    return bs;
}

int main(int argc, char** argv)
{
    using namespace phosphor::logging;
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    CLI::App app{"OpenPOWER boot control"};

    try
    {
        std::string normalOptions = "Option specify single boot step";
        // Boot step major number
        std::optional<int> major;
        app.add_option("-m,--major", major, "Step Major Number")
            ->group(normalOptions);

        // Boot step minor number
        std::optional<int> minor;
        app.add_option("-i,--minor", minor, "Step Minor Number")
            ->group(normalOptions);

        std::string rangeOptions = "Option to specify range of boot steps";

        std::optional<std::string> steps;
        app.add_option(
               "-s, --step", steps,
               "Steps to be executed, range of steps to be separated by '..'")
            ->group(rangeOptions);
        CLI11_PARSE(app, argc, argv);

        openpower::boot::Control ctrl;

        if ((minor) && (major))
        {
            ctrl.executeStep(*major, *minor);
        }
        else if (steps)
        {
            auto bs = getBootSeq(*steps);
            ctrl.executeRange(bs.first, bs.second);
        }
    }
    catch (const InternalFailure& e)
    {
        log<level::ERR>("Error in executing boot steps");
        std::cerr << "Error in executing the boot steps " << e.what()
                  << std::endl;
        commit<InternalFailure>();
        std::exit(EXIT_FAILURE);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Error in executing boot steps ",
                        entry("ERROR=%s", e.what()));
        report<InternalFailure>();
        std::exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
