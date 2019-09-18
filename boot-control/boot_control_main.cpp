#include "boot_control.hpp"

int main(int argc, char** argv)
{
    int rc = -1;
    open_power::boot::Control ctrl;
    rc = ctrl.executeStep(0, 0);
    return rc;
}
