extern "C" {
#include <gpiod.h>
}

#include "xyz/openbmc_project/Common/error.hpp"

#include <unistd.h>

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <registration.hpp>

namespace openpower
{
namespace misc
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;

/**
 * @brief Reset the CFAM using the appropriate GPIO
 * @return void
 */
void cfamReset()
{
    unsigned int offset;
    int rc;
    char chip[32];

    rc = gpiod_ctxless_find_line("cfam-reset", chip, sizeof(chip), &offset);
    if (rc != 1)
    {
        auto eno = errno;
        log<level::ERR>("failed to find cfam-reset line",
                        entry("ERRNO=%d", eno), entry("RC=%d", rc));
        throw std::system_error(eno, std::system_category());
    }

    // Put chips into reset
    rc = gpiod_ctxless_set_value(chip, offset, 0, false, "cfamReset", NULL,
                                 NULL);
    if (rc != 0)
    {
        auto eno = errno;
        log<level::ERR>("failed to set cfam-reset line to 0",
                        entry("ERRNO=%d", eno), entry("RC=%d", rc));
        throw std::system_error(eno, std::system_category());
    }

    // Sleep one second to ensure reset processed
    rc = sleep(1);
    if (rc != 0)
    {
        auto eno = errno;
        log<level::ERR>("failed to sleep for cfam-reset",
                        entry("ERRNO=%d", eno), entry("RC=%d", rc));
        throw std::system_error(eno, std::system_category());
    }

    // Take chips out of reset
    rc = gpiod_ctxless_set_value(chip, offset, 1, false, "cfamReset", NULL,
                                 NULL);
    if (rc != 0)
    {
        auto eno = errno;
        log<level::ERR>("failed to set cfam-reset line to 1",
                        entry("ERRNO=%d", eno), entry("RC=%d", rc));
        throw std::system_error(eno, std::system_category());
    }
}

REGISTER_PROCEDURE("cfamReset", cfamReset);

} // namespace misc
} // namespace openpower
