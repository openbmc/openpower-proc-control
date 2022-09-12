/**
 * Copyright © 2022 IBM Corporation
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

#include "extensions/phal/clock_logger.hpp"

#include <fmt/format.h>

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/source/event.hpp>

#include <cstdlib>

PHOSPHOR_LOG2_USING;
;

int main()
{
    try
    {
        info("Clock daily logger started");
        auto bus = sdbusplus::bus::new_default();
        auto event = sdeventplus::Event::get_default();
        openpower::phal::clock::Manager manager(event);
        bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
        return event.loop();
    }
    catch (const std::exception& ex)
    {
        error("exception during application load: ({ERROR})", "ERROR", ex);
        throw;
    }
}
