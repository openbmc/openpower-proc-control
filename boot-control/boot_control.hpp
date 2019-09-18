#pragma once

#include <stdint.h>

#include <functional>
#include <map>

namespace openpower
{
namespace boot
{

using BMCExecList = std::map<uint8_t, std::function<void(void)>>;
using BMCStepList = std::map<uint8_t, BMCExecList>;

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

  private:
    static BMCStepList bmcSteps;

    /** @brief Execute a boot step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *
     *  @error  InternalFailure exception thrown
     */
    void executeBMCStep(uint8_t stepMajor, uint8_t stepMinor);
};
} // namespace boot
} // namespace openpower
