#pragma once

namespace openpower
{
namespace p9_procedures
{

/**
 * @brief Starts the self boot engine on P9 position 0 to kick off a boot.
 * @return void
 */
void startHost();


/**
 * @brief Performs the 'VCS Workaround' on all P9s in the system.
 * @return void
 */
void vcsWorkaround();

}
}
