#include "boot_control.hpp"

namespace openpower
{
namespace boot
{

void Control::executeBMCStep(uint8_t stepMajor, uint8_t stepMinor)
{
    return;
}

void Control::executeStep(uint8_t stepMajor, uint8_t stepMinor)
{
    if (stepMajor == 0)
    {
        executeBMCStep(stepMajor, stepMinor);
    }
    return;
}
} // namespace boot
} // namespace openpower
