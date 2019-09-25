namespace open_power
{
namespace boot
{
namespace bmc_steps
{
/** @brief Apply power to the chassis.
 */
int powerOn();

/** @brief Execute SBE Config update hardware procedure
 */
int sbeConfigUpdate();

/** @brief Execute Start CBS hardware procedure
 */
int startCbs();

/** @brief An place holder for stubbed steps
 */
int stubbedStep();

} // namespace bmc_steps
} // namespace boot
} // namespace open_power
