#pragma once


#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <org/open_power/State/Boot/server.hpp>
namespace open_power
{
namespace boot
{

using BootIface = sdbusplus::server::object::object<
    sdbusplus::org::open_power::State::server::Boot>;

/** @class Manager
 *  @brief OpenPower Boot  manager implementation.
 *  @details A concrete implementation for the
 *  org::open_power::State::server::Boot::DoIpl DBus API
 */
class Manager : public BootIface
{

  public:
    Manager() = delete;
    Manager(const Manager&) = default;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;
    virtual ~Manager() = default;

    /** @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     */
    Manager(sdbusplus::bus::bus& bus, const char* path) :
        BootIface(bus, path), bus(bus)
    {
    }

    /** @brief Implementation for doIpl
     *  Method to execute an ipl step
     *  @param[in] stepMajor - step major number
     *  @param[in] stepMinor - step minor number
     */
    void doIpl(uint32_t stepMajor, uint32_t stepMinor) override;


  private:

    /** @brief sdbusplus DBus bus connection. */
    sdbusplus::bus::bus& bus;

};

} // namespace boot
} // namespace open_power
