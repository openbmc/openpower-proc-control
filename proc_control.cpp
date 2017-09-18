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
#include <algorithm>
#include <functional>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include "registration.hpp"
#include "elog-errors.hpp"
#include <org/open_power/Proc/CFAM/error.hpp>
#include <org/open_power/Proc/FSI/error.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

using namespace openpower::util;
namespace cfam = sdbusplus::org::open_power::Proc::CFAM::Error;
namespace common = sdbusplus::xyz::openbmc_project::Common::Error;
namespace fsi = sdbusplus::org::open_power::Proc::FSI::Error;

void usage(char** argv, const ProcedureMap& procedures)
{
    std::cerr << "Usage: " << argv[0] << " [action]\n";
    std::cerr << "   actions:\n";

    for (const auto& p : procedures)
    {
        std::cerr << "     " << p.first << "\n";
    }
}

int main(int argc, char** argv)
{
    using namespace phosphor::logging;
    const ProcedureMap& procedures = Registration::getProcedures();

    if (argc != 2)
    {
        usage(argv, procedures);
        return -1;
    }

    std::string action{argv[1]};

    auto procedure = procedures.find(action);

    if (procedure == procedures.end())
    {
        usage(argv, procedures);
        return -1;
    }

    try
    {
        procedure->second();
    }
    catch (cfam::SeekFailure& e)
    {
        commit<cfam::SeekFailure>();
        return -1;
    }
    catch (cfam::OpenFailure& e)
    {
        commit<cfam::OpenFailure>();
        return -1;
    }
    catch (cfam::WriteFailure& e)
    {
        commit<cfam::WriteFailure>();
        return -1;
    }
    catch (cfam::ReadFailure& e)
    {
        commit<cfam::ReadFailure>();
        return -1;
    }
    catch (common::InvalidArgument& e)
    {
        commit<common::InvalidArgument>();
        return -1;
    }
    catch (fsi::MasterDetectionFailure& e)
    {
        commit<fsi::MasterDetectionFailure>();
        return -1;
    }
    catch (fsi::SlaveDetectionFailure& e)
    {
        commit<fsi::SlaveDetectionFailure>();
        return -1;
    }


    return 0;
}
