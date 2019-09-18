#pragma once
#include <stdint.h>

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
};
} // namespace boot
} // namespace open_power
