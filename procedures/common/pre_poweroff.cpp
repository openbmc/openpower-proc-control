#include "registration.hpp"

#include <libpdbg.h>
#include <sys/wait.h>
#include <unistd.h>

#include <phosphor-logging/log.hpp>

#include <system_error>

namespace openpower
{
namespace misc
{

void prePoweroff(void)
{
    bool rc;
    log<level::INFO>("Starting prePoweroff");

    rc = ipl_pre_poweroff();
    if(rc)
    {

            log<level::ERR>("p10_pre_poweroff failed for proc");
            std::exit(EXIT_FAILURE);
    }
}

REGISTER_PROCEDURE("prePoweroff", prePoweroff)

}  // namespace misc
}  // namespace openpower
