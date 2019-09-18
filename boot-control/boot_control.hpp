#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace open_power
{
namespace boot
{

class Control
{
  public:
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    Control(Control&&) = delete;
    Control& operator=(Control&&) = delete;
    virtual ~Control() = default;

    Control()
    {
    }

    /*  @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);

  private:
    int executeBmcStep(uint8_t stepMajor, uint8_t stepMinor);
    int executeHostStep(uint8_t stepMajor, uint8_t stepMinor);
};
} // namespace boot
} // namespace open_power
