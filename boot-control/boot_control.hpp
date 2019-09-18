#pragma once
#include <stdint.h>

#include <functional>
#include <map>

namespace open_power
{
namespace boot
{

using BmcExecList = std::map<uint8_t, std::function<int(void)>>;
using BmcStepList = std::map<uint8_t, BmcExecList>;

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
    static BmcStepList bmcSteps;
  private:

    int executeBmcStep(uint8_t stepMajor, uint8_t stepMinor);
    int executeHostStep(uint8_t stepMajor, uint8_t stepMinor);
};
} // namespace boot
} // namespace open_power
