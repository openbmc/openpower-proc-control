#include "boot_control.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::Control ctrl(0,0);
    rc = ctrl.executeStep(0,0);
    return rc;
}
