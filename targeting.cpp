/**
 * Copyright © 2017 IBM Corporation
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
#include <phosphor-logging/log.hpp>
#include <regex>
#include "directory.hpp"
#include "targeting.hpp"

namespace openpower
{
namespace targeting
{

using namespace phosphor::logging;

Targeting::Targeting(const std::string& fsiMasterDev,
                     const std::string& fsiSlaveDir) :
    fsiMasterPath(fsiMasterDev),
    fsiSlaveBasePath(fsiSlaveDir)
{
    //Always create P0, the FSI master.
    auto t = std::make_shared<Target>(0, fsiMasterPath);
    targets.emplace_back(std::move(t));

    //Find the the remaining P9s dynamically based on which files show up
    Directory dir{fsiSlaveBasePath};
    std::string dirEntry;
    std::regex exp{"^slave@([0-9]{2}:00)", std::regex::extended};

    while (dir.next(dirEntry))
    {
        std::smatch match;
        if (std::regex_search(dirEntry, match, exp))
        {
            auto pos = atoi(match[1].str().c_str());
            if (pos == 0)
            {
                log<level::ERR>("Unexpected FSI slave device name found",
                                entry("DEVICE_NAME=%d", dirEntry.c_str()));
                continue;
            }

            std::string path = fsiSlaveBasePath;
            if (!(path.back() == '/'))
            {
                path += "/";
            }
            path += dirEntry;
            path += "/raw";

            auto t = std::make_shared<Target>(pos, path);
            targets.emplace_back(std::move(t));
        }
    }

    auto sortTargets = [](const std::shared_ptr<Target>& left,
                          const std::shared_ptr<Target>& right) -> bool
    {
        return left->getPos() < right->getPos();
    };
    std::sort(targets.begin(), targets.end(), sortTargets);
}

}
}
