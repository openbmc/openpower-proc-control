#include "boot_control.hpp"
#include "util.hpp"

#include <CLI/CLI.hpp>
#include <iostream>
#include <optional>

int main(int argc, char** argv)
{
    CLI::App app{"openPOWER boot control"};

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

    try
    {
        openpower::boot::util::initatdb();
    }
    catch (std::exception& e)
    {
        std::cerr << "Atdb file loading is failed" << std::endl;
        exit(-1);
    }

    openpower::boot::Control ctrl;
    if ((minor) && (major))
    {
        uint8_t major_number = 0;
        uint8_t minor_number = 0;
        try
        {
            major_number = std::stoi(*major);
            minor_number = std::stoi(*minor);
        }
        catch (std::exception& e)
        {
            std::cerr << "Parameters given are not valid" << std::endl;
            exit(-1);
        }

        try
        {
            // Call InitTagrgets expect poweron, becasuse after chassis is on
            // during poweron initTargets will invoke
            if (!((major_number == 0) && (minor_number == 0)))
            {
                openpower::boot::util::initTargets();
            }
            ctrl.executeStep(major_number, minor_number);
        }
        catch (std::exception& e)
        {
            std::cerr << "Executing the boot step failed" << std::endl;
            exit(-1);
        }
    }
    else if (steps)
    {
        int start_major = 0;
        int end_major = 0;
        auto splitPos = steps->find("..");
        if (splitPos == std::string::npos)
        {
            try
            {
                start_major = std::stoi(*steps);
                end_major = start_major;
            }
            catch (std::exception& e)
            {
                std::cerr << "Parameters given are not valid" << std::endl;
                exit(-1);
            }
        }
        else
        {
            try
            {
                start_major = std::stoi(steps->substr(0, splitPos));
                end_major = std::stoi(steps->substr(splitPos + 2));
            }
            catch (std::exception& e)
            {
                std::cerr << "Parameters given are not valid" << std::endl;
                exit(-1);
            }
        }

        try
        {
            // Call InitTagrgets expect poweron, becasuse after chassis is on
            // during poweron initTargets will invoke
            if (start_major > 0)
            {
                openpower::boot::util::initTargets();
            }
            ctrl.executeRange(start_major, end_major);
        }
        catch (std::exception& e)
        {
            std::cerr << "Executing the boot steps failed" << std::endl;
            exit(-1);
        }
    }

    exit(0);
}
