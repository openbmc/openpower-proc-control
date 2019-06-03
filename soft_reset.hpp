#pragma once

#include <xyz/openbmc_project/Control/Host/NMI/server.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>

namespace openpower
{
namespace proc
{

using Base = sdbusplus::xyz::openbmc_project::Control::Host::server::NMI;
using Interface = sdbusplus::server::object::object<Base>;
namespace sdbusRule = sdbusplus::bus::match::rules;


/*  @class SoftReset
 *  @brief Implementation of SoftReset (SRESET PROC)
 */
class SoftReset : public Interface
{
  public:
    SoftReset() = delete;
    SoftReset(const SoftReset&) = delete;
    SoftReset& operator=(const SoftReset&) = delete;
    SoftReset(SoftReset&&) = delete;
    SoftReset& operator=(SoftReset&&) = delete;
    virtual ~SoftReset() = default;

    /*  @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - sdbusplus D-Bus to attach to.
     *  @param[in] path - Path to attach to.
     */
    SoftReset(sdbusplus::bus::bus& bus, const char* path);

    /*  @brief do sreset.
     */
    void nmiReset() override;
  private:

    /** @brief sdbus handle */
    sdbusplus::bus::bus& bus;

    /** @brief object path */
    std::string objectPath;

};

} // namespace proc
} // namespace open_power

