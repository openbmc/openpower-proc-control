#include "bmc_boot_steps.hpp"

#include <iostream>

namespace open_power
{
namespace boot
{
namespace bmc_steps
{

int StubbedStep()
{

    std::cout <<"Step is a stub"<<std::endl;
    return 0;
}
} // bmc_steps
} // boot
} // open_power
