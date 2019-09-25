namespace open_power
{
namespace boot
{
namespace bmc_steps
{
/** @brief Apply power to the chassis.
 */
int powerOn();

/** @brief An place holder for stubbed steps
 */
int StubbedStep();

/** @brief Execute SBE Config update hardware procedure
 */
int SbeConfigUpdate();

/** @brief Execute Start CBS hardware procedure
 */
int StartCbs();

} // namespace bmc_steps
} // namespace boot
} // namespace open_power
