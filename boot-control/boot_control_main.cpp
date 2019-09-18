#include "boot_control.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <CLI/CLI.hpp>
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>

int main(int argc, char** argv)
{
    using namespace phosphor::logging;
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    CLI::App app{"OpenPOWER boot control"};

    // Boot step major number
    int major;
    app.add_option("-m,--major", major, "Step Major Number")->required();

    // Boot step minor number
    int minor;
    app.add_option("-i,--minor", minor, "Step Minor Number")->required();
    CLI11_PARSE(app, argc, argv);

    try
    {
        openpower::boot::Control ctrl;
        ctrl.executeStep(major, minor);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error in executing the step " << e.what() << std::endl;
        report<InternalFailure>();
        std::exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
