#pragma once

#include <stdint.h>

#include <functional>
#include <map>

namespace openpower
{
namespace boot
{

using BmcExecList = std::map<uint8_t, std::function<void(void)>>;
using BmcStepList = std::map<uint8_t, BmcExecList>;

/** @class Control
 *  @brief Implements boot step control
 *  @details Entry point for executing boot steps and execute the step on
 *  the right subsystem.
 */
class Control
{
  public:
    Control() = default;
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    Control(Control&&) = delete;
    Control& operator=(Control&&) = delete;
    virtual ~Control() = default;

    /** @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *
     *  @error  InternalFailure exception thrown
     */
    void executeStep(uint8_t stepMajor, uint8_t stepMinor);
    static BmcStepList bmcSteps;

  private:
    /** @brief Execute a boot step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *
     *  @error  InternalFailure exception thrown
     */
    void executeBmcStep(uint8_t stepMajor, uint8_t stepMinor);
};
} // namespace boot
} // namespace openpower
