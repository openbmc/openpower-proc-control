#include "argument.hpp"
#include "boot_control.hpp"
#include "util.hpp"

int main(int argc, char** argv)
{
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    int rc = -1;
    if ((rc = open_power::boot::util::initatdb()) != 0)
        return rc;

    open_power::boot::Control ctrl;

    // Faster execution if only one step is specified
    if (opt.singleStep)
    {
        // Init targets will call after chassis is on while execute poweron step
        if (!((opt.start_major == 0) && (opt.end_minor == 0)))
            open_power::boot::util::initTargets();

        return ctrl.executeStep(opt.start_major, opt.start_minor);
    }
    // else execute the range
    // Init target will invoke after chassin is on during poweron step in step
    // 0. So, If step range executed from 1 then init targets should call
    if (opt.start_major != 0)
    {
        // Init targets only if chassin is on
        if (open_power::boot::util::isChassisOn())
            open_power::boot::util::initTargets();
    }

    return ctrl.executeRange(opt.start_major, opt.end_major);
}
