#include "boot_control.hpp"
#include "argument.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    open_power::boot::Control ctrl(0,0);
    rc = ctrl.executeStep(opt.start_major, opt.start_minor);
    return rc;
}
