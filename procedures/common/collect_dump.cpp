/**
 * Copyright (C) 2020 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "registration.hpp"

#include <libpdbg.h>
#include <sys/wait.h>
#include <unistd.h>

#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

#include <chrono>
#include <filesystem>
#include <system_error>
#include <vector>

#include <iostream>

namespace openpower
{
namespace misc
{

constexpr auto DUMP_FILES_PATH = "/tmp/";
/**
 * @brief Calls sbe_dump on the SBE in the provided target.
 * @param[in] tgt - Proc target which contains the SBE
 * @param[in] timestamp - Timestamp to differentiate between dumps
 * @return void
 */
void collectHbDump(struct pdbg_target* tgt, uint64_t timestamp)
{
    using namespace sdbusplus::xyz::openbmc_project::Common::Error;
    using namespace sdbusplus::xyz::openbmc_project::Common::File::Error;
    using namespace phosphor::logging;

    using ErrnoOpen = xyz::openbmc_project::Common::File::Open::ERRNO;
    using PathOpen = xyz::openbmc_project::Common::File::Open::PATH;
    using ErrnoWrite = xyz::openbmc_project::Common::File::Write::ERRNO;
    using PathWrite = xyz::openbmc_project::Common::File::Write::PATH;
    
    int error = 0;
    uint8_t *data = NULL;
    uint32_t len = 0;

    uint32_t chipPos = 0;
    pdbg_target_get_attribute(tgt, "ATTR_FAPI_POS", 4, 1, &chipPos);
    std::string filename = "hbdump_" + std::to_string(timestamp) + "_" + std::to_string(chipPos);
    std::filesystem::path dumpPath(DUMP_FILES_PATH);
    dumpPath /= std::to_string(timestamp);
    dumpPath /= filename;
    
    if ((error = sbe_dump(tgt, 0, 0, &data, &len )) < 0)
    {
        log<level::ERR>("Failed to initiate memory preserving reboot");
        // TODO Create a PEL in the future for this failure case.
        throw std::system_error(error, std::generic_category(),
                                "Failed to initiate memory preserving reboot");
    }

    if (len == 0)
    {
        log<level::ERR>("No data returned while collecting the dump");
        // TODO Create a PEL in the future for this failure case.
        throw std::system_error(error, std::generic_category(),
                                "No data returned while collecting the dump");
    }
    std::cout << "size:" << len <<std::endl;

    std::ofstream outfile{dumpPath, std::ios::out | std::ios::binary};
    if (!outfile.good())
    {
        // Unable to open the file for writing
        auto err = errno;
        log<level::ERR>("Error opening file to write dump ",
                        entry("ERR=%d", errno),
                        entry("FILEPATH=%s", dumpPath.c_str()));
        elog<Open>(ErrnoOpen(err), PathOpen(dumpPath.c_str()));
    }

    outfile.exceptions(std::ifstream::failbit | std::ifstream::badbit |
                       std::ifstream::eofbit);

    try
    {
        outfile.write(reinterpret_cast<char *>(data), len);
        outfile.close();
    }
    catch (std::ofstream::failure& oe)
    {
        auto err = errno;
        log<level::ERR>("Failed to write to dump file ",
                        entry("ERR=%s", oe.what()),
                        entry("FILEPATH=%s", dumpPath.c_str()));
        elog<Write>(ErrnoWrite(err), PathWrite(dumpPath.c_str()));
    }
    
    log<level::INFO>("Hostboot dump collected");
}

/**
 * @brief initiate memory preserving reboot on each SBE.
 * @return void
 */
void collectHostbootDump()
{
    using namespace phosphor::logging;
    struct pdbg_target* target;
    std::vector<pid_t> pidList;
    bool failed = false;
    pdbg_targets_init(NULL);
    pdbg_set_loglevel(PDBG_DEBUG);
    auto timeNow = std::chrono::steady_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(timeNow.time_since_epoch()).count();

    log<level::INFO>("Starting hostboot dump collection");
    pdbg_for_each_class_target("pib", target)
    {
        if (pdbg_target_probe(target) != PDBG_TARGET_ENABLED)
        {
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            log<level::ERR>("Fork failed while starting hostboot dump collection");
            failed = true;
        }
        else if (pid == 0)
        {
            collectHbDump(target, timestamp);
            std::exit(EXIT_SUCCESS);
        }
        else
        {
            pidList.push_back(std::move(pid));
        }
    }

    for (auto& p : pidList)
    {
        int status = 0;
        waitpid(p, &status, 0);
        if (WEXITSTATUS(status))
        {
            log<level::ERR>("Hostboot dump collection failed");
            failed = true;
        }

        log<level::ERR>("Dump collection completed");
    }

    if (failed)
    {
        std::exit(EXIT_FAILURE);
    }
}

REGISTER_PROCEDURE("collectHostbootDump", collectHostbootDump)

} // namespace misc
} // namespace openpower
