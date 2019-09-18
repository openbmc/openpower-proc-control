#include "argument.hpp"
#include "boot_control.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    open_power::boot::Control ctrl;

    // Faster execution if only one step is specified
    if ((opt.start_major == opt.end_major) &&
        (opt.start_minor == opt.end_minor) && (opt.start_minor != 0xFF))
    {
        return ctrl.executeStep(opt.start_major, opt.start_minor);
    }
    // else execute the range
    return ctrl.executeRange(opt.start_major, opt.end_major);
}
