#pragma once
#include <iostream>
#include <map>
#include <functional>
#include "bmc_boot_steps.hpp"
namespace open_power
{
namespace boot
{

/* @enum ExecStepType
 * @brief The type of step execution
 */
enum class ExecStepType
{
    BMC_STEP, //Execute by BMC
    SBE_STEP, //Execute by SBE
    HB_STEP   //Execute by Hostboot
};

using MajorStepsList = std::map<uint8_t, ExecStepType>;

using BmcExecList = std::map<uint8_t,
                       std::function<void(void)>>;
using BmcStepList = std::map<uint8_t, BmcExecList>;

/*  @class BmcStep
 *  @brief Implementation of BMC step control
 */
class BmcStep
{
  public:
    BmcStep() = delete;
    BmcStep(const BmcStep&) = delete;
    BmcStep& operator=(const BmcStep&) = delete;
    BmcStep(BmcStep&&) = delete;
    BmcStep& operator=(BmcStep&&) = delete;
    virtual ~BmcStep() = default;

    /*  @brief Execute a step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */   
    static void executeStep(uint8_t stepMajor, uint8_t stepMinor);
};

/*  @class SbeStep
 *  @brief Implementation of SBE step control
 */
class SbeStep
{
  public:
    SbeStep() = delete;
    SbeStep(const SbeStep&) = delete;
    SbeStep& operator=(const SbeStep&) = delete;
    SbeStep(SbeStep&&) = delete;
    SbeStep& operator=(SbeStep&&) = delete;
    virtual ~SbeStep() = default;

    /*  @brief Execute a step in SBE.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    static void executeStep(uint8_t stepMajor, uint8_t stepMinor);
};

/*  @class HostbootStep
 *  @brief Implementation of Hostboot step control
 */
class HostbootStep
{
  public:
    HostbootStep() = delete;
    HostbootStep(const HostbootStep&) = delete;
    HostbootStep& operator=(const HostbootStep&) = delete;
    HostbootStep(HostbootStep&&) = delete;
    HostbootStep& operator=(HostbootStep&&) = delete;
    virtual ~HostbootStep() = default;

    /*  @brief Execute a step in Hostboot.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    static void executeStep(uint8_t stepMajor, uint8_t stepMinor);
};

class Control
{
  public:
    Control() = delete;
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    Control(Control&&) = delete;
    Control& operator=(Control&&) = delete;
    virtual ~Control() = default;

    /*  @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    static void executeStep(uint8_t stepMajor, uint8_t stepMinor);

  private:
    /*  @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    template <class T>
    static void execute(uint8_t stepMajor, uint8_t stepMinor)
    {
        T::executeStep(stepMajor, stepMinor);

    }
};
}
}
