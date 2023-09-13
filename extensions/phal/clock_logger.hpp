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
#pragma once

#include "extensions/phal/create_pel.hpp"

#include <sdeventplus/utility/timer.hpp>

#include <chrono>
#include <format>

namespace openpower::phal::clock
{

/* Dbus event timer */
using Timer = sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic>;

/**
 * @class Manager - Represents the clock daily management functions
 *
 */
class Manager
{
  public:
    Manager() = delete;
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    ~Manager() = default;

    /**
     * Constructor
     * Starts the functions required to capture clock daily logger data.
     *
     * @param[in] event - sdeventplus event loop
     */
    Manager(const sdeventplus::Event& event);

    /**
     * @brief Add a dbus timer
     */
    void addTimer();

    /**
     * @brief Callback when a timer expires
     */
    void timerExpired();

    /**
     * @brief utility function to create clock data log
     */
    void createClockDataLog();

    /**
     * @brief Add processor specific CFAM data to daily logger.
     *
     * @param[in] proc - pdbg processor target
     * @param[out] ffdcData - reference to clock data log
     */
    void addCFAMData(struct pdbg_target* proc,
                     openpower::pel::FFDCData& clockDataLog);

    /**
     * @brief Add clock specific register data to daily logger.
     *
     * @param[out] ffdcData - reference to clock data log
     */
    void addClockRegData(openpower::pel::FFDCData& clockDataLog);

  private:
    /* The sdeventplus even loop to use */
    sdeventplus::Event _event;

    /** @brief Timer used for LEDs lamp test period */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;
};

} // namespace openpower::phal::clock
