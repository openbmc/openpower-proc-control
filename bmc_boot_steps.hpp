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

    /*
     * Apply power to the chassis.
     */
     static int PowerOn();

    /*  @brief Get BMC to a state to start the boot.
     */
    static int StartIpl();

    /*  @brief Adjust the reference clock frequencies.
     */
    static int SetRefClock();

    /*  @brief Test to see whether ref clock is valid.
     */
    static int ProcClockTest();

    /*  @brief Prepare for boot.
     */
    static int ProcPrepIpl();

    /*  @brief Select the SEEPROM to use.
     */
    static int ProcSelectBootMater();

    /*  @brief Update the SBE config.
     */
    static int SbeConfigUpdate();

    /*  @brief Start SBE to start the hostboot.
     */
    static int SbeStart();

    /*  @brief Stubbed steps.
     */
    static int StubbedStep(const char  *des);

    /*  @brief No-op steps on BMC.
     */
    static int NoopStep()
    { return 0;}
};
}//boot
}//open_power
