#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace open_power
{
namespace boot
{

/* @enum ExecStepType
 * @brief The type of step execution
 */
enum class ExecStepType
{
    BMC_STEP, // Execute by BMC
    SBE_STEP, // Execute by SBE
    HB_STEP   // Execute by Hostboot
};

using MinorStepList = std::map<uint8_t, std::string>;

struct StepInfo
{
    ExecStepType stepType;
    MinorStepList minorStepList;
};

using MajorStepsList = std::map<uint8_t, StepInfo>;

using BmcExecList = std::map<uint8_t, std::function<int(void)>>;
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
    BmcStep(int mode, int type):mode(mode),type(type)
    {
    }

    /*  @brief Execute a step in BMC.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);
  private:
    int mode;
    int type;
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
    SbeStep(int mode, int type):mode(mode),type(type)
    {
    }

    /*  @brief Execute a step in SBE.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);
  private:
    int mode;
    int type;
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
    HostbootStep(int mode, int type):mode(mode),type(type)
    {
    }

    /*  @brief Execute a step in Hostboot.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);
  private:
    int mode;
    int type;
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

    Control(int mode, int type):mode(mode),type(type)
    {
        bmcExec = std::shared_ptr<BmcStep>(new BmcStep(mode, type));
        sbeExec = std::shared_ptr<SbeStep>(new SbeStep(mode, type));
        hbExec  = std::shared_ptr<HostbootStep>(new HostbootStep(mode, type));
    }

    /*  @brief Execute a boot step.
     *  @param[in] stepMajor - a Major boot step.
     *  @param[in] stepMinor - a Minor boot step or substep.
     */
    int executeStep(uint8_t stepMajor, uint8_t stepMinor);
    int executeRange(uint8_t startStep, uint8_t endStep);
    static MajorStepsList majorSteps;

  private:
     int mode;
     int type;
     std::shared_ptr<BmcStep> bmcExec;
     std::shared_ptr<SbeStep> sbeExec;
     std::shared_ptr<HostbootStep>  hbExec;
};
} // namespace boot
} // namespace open_power
