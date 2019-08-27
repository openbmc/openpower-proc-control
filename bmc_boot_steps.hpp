namespace open_power
{
namespace boot
{

/*  @class BmcExecutor
 *  @brief Implementation of BMC boot steps
 */
class BmcExecutor
{
  public:
    /*  @brief Get BMC to a state to start the boot.
     */ 
    static void StartIpl();

    /*  @brief Adjust the reference clock frequencies.
     */
    static void SetRefClock();

    /*  @brief Test to see whether ref clock is valid.
     */
    static void ProcClockTest();

    /*  @brief Prepare for boot.
     */
    static void ProcPrepIpl();

    /*  @brief Select the SEEPROM to use.
     */
    static void ProcSelectBootMater();

    /*  @brief Update the SBE config.
     */
    static void SbeConfigUpdate();

    /*  @brief Start SBE to start the hostboot.
     */
    static void SbeStart();

    /*  @brief Stubbed steps.
     */
    static void StubbedStep(const char  *des);

    /*  @brief No-op steps on BMC.
     */
    static void NoopStep()
    {} 
};
}//boot
}//open_power
