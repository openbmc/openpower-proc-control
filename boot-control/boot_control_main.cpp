#include "argument.hpp"
#include "boot_control.hpp"
#include "util.hpp"

int main(int argc, char** argv)
{
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    int rc = -1;
    if ((rc = open_power::boot::util::initatdb()) != 0)
    {
        return rc;
    }

    // Call InitTagrgets expect poweron, becasuse after chassis is on during
    // poweron initTargets will invoke
    bool callInitTargets = false;
    if (opt.singleStep)
    {
        if (!((opt.start_major == 0) && (opt.start_minor == 0)))
        {
            callInitTargets = true;
        }
    }
    else if (opt.start_major > 0)
    {
        callInitTargets = true;
    }

    if (callInitTargets)
    {
        // Init targets only if chassis is on
        if (open_power::boot::util::isChassisOn())
        {
            if (0 != open_power::boot::util::initTargets())
            {
                return -1;
            }
        }
    }

    open_power::boot::Control ctrl;

    // Faster execution if only one step is specified
    if (opt.singleStep)
    {
        return ctrl.executeStep(opt.start_major, opt.start_minor);
    }
    // else execute the range
    return ctrl.executeRange(opt.start_major, opt.end_major);
}
