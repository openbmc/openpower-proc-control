#include "boot_control.hpp"
#include "argument.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    open_power::boot::Control ctrl(0,0);

    //Faster execution if only one step is specified
    if ((opt.start_major == opt.end_major) &&
        (opt.start_minor == opt.end_minor) &&
        (opt.start_minor != 0xFF))
    {
        return ctrl.executeRange(opt.start_major, opt.start_minor);
    } 
    rc = ctrl.executeStep(opt.start_major, opt.end_major);
    return rc;
}
