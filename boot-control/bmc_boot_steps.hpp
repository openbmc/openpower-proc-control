namespace openpower
{
namespace boot
{
namespace bmc_steps
{
/** @brief Apply power to the chassis.
 */
void powerOn();

/** @brief Execute SBE Config update hardware procedure
 */
void sbeConfigUpdate();

/** @brief Execute Start CBS hardware procedure
 */
void startCbs();

/** @brief An place holder for stubbed steps
 */
void stubbedStep();
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
