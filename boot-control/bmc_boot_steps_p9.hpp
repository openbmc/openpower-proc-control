namespace openpower
{
namespace boot
{
namespace bmc_steps
{
/** @brief Apply power to the chassis.
 */
void powerOn();

/** @brief Execute Start SBE hardware procedure
 */
void startSbe();

/** @brief Execute SbeConfigUpdate hardware procedure
 */
void sbeConfigUpdate();

/** @brief An place holder for stubbed steps
 */
void stubbedStep();
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
