#include "argument.hpp"
#include "boot_control.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::util::optstruct opt;
    open_power::boot::util::parseArguments(argc, argv, opt);

    //Faster execution if only one step is specified
    if ((opt.start_major == opt.end_major) &&
        (opt.start_minor == opt.end_minor) &&
        (opt.start_minor != 0xFF))
    {
        rc = open_power::boot::Control::executeStep(opt.start_major,
                                                    opt.start_minor);
        return rc;
    }

    //For range of isteps
    bool completed = false;
    auto begin_step =
        open_power::boot::Control::majorSteps.find(opt.start_major);
    if (begin_step == open_power::boot::Control::majorSteps.end())
    {
        std::cout << "Invalid step :"<< opt.start_major <<std::endl;
    }

    for (auto iter = begin_step;
         iter != open_power::boot::Control::majorSteps.end(); iter++)
    {
        open_power::boot::MinorStepList::iterator miter;
        if ((iter->first == opt.start_major) && (opt.start_minor != 0xFF))
        {
            miter = iter->second.minorStepList.find(opt.start_minor);
        }
        else
        {
            miter = iter->second.minorStepList.begin();
        }
        for (; miter!= iter->second.minorStepList.end(); miter++)
        {
            rc = open_power::boot::Control::executeStep(iter->first,
                                                        miter->first);
            if (rc < 0)
            {
                return rc;
            }
            if ((iter->first == opt.end_major) &&
                (miter->first == opt.end_minor))
            {
                completed = true;
                break;
            }
        }
        if ((completed) || (iter->first == opt.end_major))
        {
            break;
        }
    }
}
