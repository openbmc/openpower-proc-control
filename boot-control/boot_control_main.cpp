#include "boot_control.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    openpower::boot::Control ctrl;
    try
    {
        ctrl.executeStep(0, 0);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error in executing the step " << e.what() << std::endl;
        exit(-1);
    }
    exit(0);
}
