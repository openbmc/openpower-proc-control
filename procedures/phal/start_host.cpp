extern "C" {
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"

#include <libekb.H>
#include <libipl.H>

#include <ext_interface.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>

#include "attributes_info.H"
namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

/**
 *  @brief Enables select_boot_master() on POWER processor possition position
 * 0/1 depending on boot count before kicking off the boot.
 *  @return void
 */
void selectBootMaster()
{
    struct pdbg_target* procTarget;
    ATTR_BACKUP_SEEPROM_SELECT_Enum bkpSeePromSelect;
    ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT_Enum bkpMeaSeePromSelect;

    pdbg_for_each_class_target("proc", procTarget)
    {
        if (pdbg_target_index(procTarget) != 0)
            continue;

        // Choose seeprom side to boot from
        if (getBootCount() > 0)
        {
            log<level::INFO>("Setting SBE seeprom side to 0",
                             entry("SBE_SIDE_SELECT=%d", 0));

            bkpSeePromSelect = ENUM_ATTR_BACKUP_SEEPROM_SELECT_PRIMARY;
            if (DT_SET_PROP(ATTR_BACKUP_SEEPROM_SELECT, procTarget,
                            bkpSeePromSelect))
            {
                log<level::ERR>(
                    "Attribute [ATTR_BACKUP_SEEPROM_SELECT] set failed");
                openpower::pel::detail::processBootErrorCallback(false);
                throw std::runtime_error(
                    "Attribute [ATTR_BACKUP_SEEPROM_SELECT] set failed");
            }
            // To clear trace if success
            openpower::pel::detail::processBootErrorCallback(true);

            bkpMeaSeePromSelect =
                ENUM_ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT_PRIMARY;
            if (DT_SET_PROP(ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT, procTarget,
                            bkpMeaSeePromSelect))
            {
                log<level::ERR>(
                    "Attribute [ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT] set "
                    "failed");
                openpower::pel::detail::processBootErrorCallback(false);
                throw std::runtime_error(
                    "Attribute [ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT] set "
                    "failed");
            }
            // To clear trace if success
            openpower::pel::detail::processBootErrorCallback(true);
        }
        else
        {
            log<level::INFO>("Setting SBE seeprom side to 1",
                             entry("SBE_SIDE_SELECT=%d", 1));
            bkpSeePromSelect = ENUM_ATTR_BACKUP_SEEPROM_SELECT_SECONDARY;
            if (DT_SET_PROP(ATTR_BACKUP_SEEPROM_SELECT, procTarget,
                            bkpSeePromSelect))
            {
                log<level::ERR>(
                    "Attribute [ATTR_BACKUP_SEEPROM_SELECT] set failed");
                openpower::pel::detail::processBootErrorCallback(false);
                throw std::runtime_error(
                    "Attribute [ATTR_BACKUP_SEEPROM_SELECT] set failed");
            }
            // To clear trace if success
            openpower::pel::detail::processBootErrorCallback(true);

            bkpMeaSeePromSelect =
                ENUM_ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT_SECONDARY;
            if (DT_SET_PROP(ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT, procTarget,
                            bkpMeaSeePromSelect))
            {
                log<level::ERR>(
                    "Attribute [ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT] set "
                    "failed");
                openpower::pel::detail::processBootErrorCallback(false);
                throw std::runtime_error(
                    "Attribute [ATTR_BACKUP_MEASUREMENT_SEEPROM_SELECT] set "
                    "failed");
            }
            // To clear trace if success
            openpower::pel::detail::processBootErrorCallback(true);
        }
    }
}

/**
 * @brief Starts the self boot engine on POWER processor position 0
 *        to kick off a boot.
 * @return void
 */
void startHost()
{
    // add callback methods for debug traces and for boot failures
    openpower::pel::addBootErrorCallbacks();

    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("pdbg target initialization failed");
    }
    // To clear trace if success
    openpower::pel::detail::processBootErrorCallback(true);

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("libekb initialization failed");
    }
    // To clear trace if success
    openpower::pel::detail::processBootErrorCallback(true);

    if (ipl_init(IPL_AUTOBOOT) != 0)
    {
        log<level::ERR>("ipl_init failed");
        openpower::pel::detail::processBootErrorCallback(false);
        throw std::runtime_error("Boot initialization failed");
    }
    // To clear trace if success
    openpower::pel::detail::processBootErrorCallback(true);

    // Run select boot master before poweron
    selectBootMaster();

    // callback method will be called upon failure which will create the PEL
    int rc = ipl_run_major(0);
    if (rc > 0)
    {
        log<level::ERR>("step 0 failed to start the host");
        throw std::runtime_error("Failed to execute host start boot step");
    }
}

REGISTER_PROCEDURE("startHost", startHost);

} // namespace phal
} // namespace openpower
