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
    std::string major;
    app.add_option("-m,--major", major, "Step Major Number")->required();

    // Boot step minor number
    std::string minor;
    app.add_option("-i,--minor", minor, "Step Minor Number")->required();
    CLI11_PARSE(app, argc, argv);

    uint8_t major_number = 0;
    uint8_t minor_number = 0;
    try
    {
        major_number = std::stoi(major);
        minor_number = std::stoi(minor);

        openpower::boot::Control ctrl;
        ctrl.executeStep(major_number, minor_number);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error in executing the step " << e.what() << std::endl;
        report<InternalFailure>();
        exit(-1);
    }
    exit(0);
}
