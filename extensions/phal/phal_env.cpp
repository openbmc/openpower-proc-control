// SPDX-License-Identifier: Apache-2.0

#include "extensions/phal/phal_env.hpp"

#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{
namespace env
{

using namespace phosphor::logging;

void setDevtreeEnv()
{
    // PDBG_DTB environment variable set to CEC device tree path
    static constexpr auto PDBG_DTB_PATH =
        "/var/lib/phosphor-software-manager/pnor/rw/DEVTREE";

    if (setenv("PDBG_DTB", PDBG_DTB_PATH, 1))
    {
        log<level::ERR>(
            fmt::format("Failed to set PDBG_DTB: ({})", strerror(errno))
                .c_str());
        throw std::runtime_error("Failed to set PDBG_DTB");
    }
}

void setPdataInfoDBEnv()
{
    // PDATA_INFODB environment variable set to attributes tool infodb path
    static constexpr auto PDATA_INFODB_PATH =
        "/usr/share/pdata/attributes_info.db";

    if (setenv("PDATA_INFODB", PDATA_INFODB_PATH, 1))
    {
        log<level::ERR>(
            fmt::format("Failed to set PDATA_INFODB: ({})", strerror(errno))
                .c_str());
        throw std::runtime_error("Failed to set PDATA_INFODB");
    }
}

} // namespace env
} // namespace phal
} // namespace openpower
