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

using StepName = std::string;
using MajorStepNumber = uint8_t;
using MinorStepNumber = uint8_t;
using MinorStepList = std::map<MinorStepNumber, StepName>;
using MajorStepsList = std::map<MajorStepNumber, MinorStepList>;

class Control
{
  public:
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    Control(Control&&) = delete;
    Control& operator=(Control&&) = delete;
    virtual ~Control() = default;

    /*  @brief Constructor for the class
     */
    Control()
    {
    }

    /*  @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *  @return    zero if success.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);

    /*  @brief Execute a range of boot step.
     *  @param[in] startStep - a Major start boot step.
     *  @param[in] stepMinor - a Major end boot step.
     */
    int executeRange(uint8_t startStep, uint8_t endStep);

    static BmcStepList bmcSteps;
    MajorStepsList majorSteps;

  private:
    /*  @brief Execute a boot step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *  @return    zero if success.
     */
    int executeBmcStep(uint8_t stepMajor, uint8_t stepMinor);

    /*  @brief Execute a boot step in SBE or Hostboot.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     *  @return    zero if success.
     */
    int executeHostStep(uint8_t stepMajor, uint8_t stepMinor);
    int loadSteps();
};
} // namespace boot
} // namespace open_power
