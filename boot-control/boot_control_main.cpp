#include "boot_control.hpp"
#include "pdbg_wrapper.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <CLI/CLI.hpp>
#include <iostream>
#include <optional>
#include <phosphor-logging/elog-errors.hpp>

int main(int argc, char** argv)
{
    using namespace phosphor::logging;
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    CLI::App app{"OpenPOWER boot control"};

    std::string normalOptions = "Option specify single boot step";
    // Boot step major number
    std::optional<std::string> major;
    app.add_option("-m,--major", major, "Step Major Number")
        ->group(normalOptions);

    // Boot step minor number
    std::optional<std::string> minor;
    app.add_option("-i,--minor", minor, "Step Minor Number")
        ->group(normalOptions);

    std::string rangeOptions = "Option to specify range of boot steps";

    std::optional<std::string> steps;
    app.add_option(
           "-s, --step", steps,
           "Steps to be exceuted, range of steps to be seperated by '..'")
        ->group(rangeOptions);
    CLI11_PARSE(app, argc, argv);

    openpower::boot::Control ctrl;

    try
    {
        openpower::boot::util::pdbg::initatdb();
        openpower::boot::util::pdbg::initTargets();

        if ((minor) && (major))
        {
            uint8_t major_number = 0;
            uint8_t minor_number = 0;
            major_number = std::stoi(*major);
            minor_number = std::stoi(*minor);

            ctrl.executeStep(major_number, minor_number);
        }
        else if (steps)
        {
            int start_major = 0;
            int end_major = 0;
            auto splitPos = steps->find("..");
            if (splitPos == std::string::npos)
            {
                start_major = std::stoi(*steps);
                end_major = start_major;
            }
            else
            {
                start_major = std::stoi(steps->substr(0, splitPos));
                end_major = std::stoi(steps->substr(splitPos + 2));
            }

            ctrl.executeRange(start_major, end_major);
        }
    }
    catch (const InternalFailure& e)
    {
        std::cerr << "Error in executing the boot steps " << e.what()
                  << std::endl;
        commit<InternalFailure>();
        exit(-1);
    }
    catch (std::exception& e)
    {
        std::cerr << "Executing the boot steps failed " << e.what()
                  << std::endl;
        report<InternalFailure>();
        exit(-1);
    }

    exit(0);
}
