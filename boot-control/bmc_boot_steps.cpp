#include "bmc_boot_steps.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace boot
{
namespace bmc_steps
{
using namespace phosphor::logging;

void stubbedStep()
{
    log<level::INFO>("Step is a stub");
}
} // namespace bmc_steps
} // namespace boot
} // namespace openpower
