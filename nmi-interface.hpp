#pragma once

#include <xyz/openbmc_project/Control/Host/NMI/server.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <sys/inotify.h>
#include <systemd/sd-event.h>

namespace openpower
{
namespace proc
{

using Base = sdbusplus::xyz::openbmc_project::Control::Host::server::NMI;
using Interface = sdbusplus::server::object::object<Base>;
namespace sdbusRule = sdbusplus::bus::match::rules;

/* Need a custom deleter for freeing up sd_event */
struct EventDeleter
{
    void operator()(sd_event* event) const
    {
        event = sd_event_unref(event);
    }
};

using EventPtr = std::unique_ptr<sd_event, EventDeleter>;


/*  @class NMI
 *  @brief Implementation of NMI (SRESET PROC)
 */
class NMI : public Interface
{
  public:
    NMI() = delete;
    NMI(const NMI&) = delete;
    NMI& operator=(const NMI&) = delete;
    NMI(NMI&&) = delete;
    NMI& operator=(NMI&&) = delete;
    virtual ~NMI() = default;

    /*  @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - sdbusplus D-Bus to attach to.
     *  @param[in] path - Path to attach to.
     */
    //NMI(sdbusplus::bus::bus& bus, const char* path);


    NMI(sdbusplus::bus::bus& bus, const EventPtr& event, const char* path) :
        Interface(bus, path), bus(bus), eventP(event.get())
    {
    }

    /*  @brief do sreset.
     */
    void nmiReset() override;
  private:

    /** @brief sdbus handle */
    sdbusplus::bus::bus& bus;
    
    /** @brief sdbusplus event loop */
    EventPtr eventP;

    /** @brief object path */
    std::string objectPath;

};

} // namespace proc
} // namespace open_power

