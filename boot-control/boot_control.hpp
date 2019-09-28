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

using StepName = std::string;
using MajorStepNumber = uint8_t;
using MinorStepNumber = uint8_t;
using MinorStepList = std::map<MinorStepNumber, StepName>;
using MajorStepsList = std::map<MajorStepNumber, MinorStepList>;

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

    /**  @brief Execute a range of boot step.
     *   @param[in] startStep - a Major start boot step.
     *   @param[in] stepMinor - a Major end boot step.
     *
     *   @error  InternalFailure exception thrown
     */
    void executeRange(uint8_t startStep, uint8_t endStep);

  private:
    static BMCStepList bmcSteps;
    static MajorStepsList majorSteps;

    /** @brief Execute a boot step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *
     *  @error  InternalFailure exception thrown
     */
    void executeBMCStep(uint8_t stepMajor, uint8_t stepMinor);

    /** @brief Execute a boot step in SBE or Hostboot.
     *  @details execute the steps in respective host subsystem.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *
     *  @error  InternalFailure exception thrown
     */
    void executeHostStep(uint8_t stepMajor, uint8_t stepMinor);
};
} // namespace boot
} // namespace openpower
