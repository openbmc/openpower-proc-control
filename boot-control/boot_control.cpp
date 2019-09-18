#include "boot_control.hpp"

namespace open_power
{
namespace boot
{
int Control::executeHostStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
}

int Control::executeBmcStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return 0;
}

int Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    int rc = -1;
    if (stepMajor == 0)
    {
        rc = executeBmcStep(stepMajor, stepMinor);
    }
    else
    {
        rc = executeHostStep(stepMajor, stepMinor);
    }
    return rc;
}
} // namespace boot
} // namespace open_power
