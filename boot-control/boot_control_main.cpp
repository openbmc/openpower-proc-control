#include "argument.hpp"
#include "boot_control.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    open_power::boot::Control ctrl;
    rc = ctrl.executeStep(opt.start_major, opt.start_minor);
    return rc;
}
