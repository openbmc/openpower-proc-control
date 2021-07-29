#include "create_pel.hpp"

#include "util.hpp"

#include <fcntl.h>
#include <fmt/format.h>
#include <libekb.H>
#include <unistd.h>

#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Logging/Create/server.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace openpower
{
using namespace phosphor::logging;

namespace pel
{

constexpr auto loggingObjectPath = "/xyz/openbmc_project/logging";
constexpr auto loggingInterface = "xyz.openbmc_project.Logging.Create";

void createBootErrorPEL(const FFDCData& ffdcData, const json& calloutData)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }

    try
    {
        FFDCFile ffdcFile(calloutData);

        std::vector<std::tuple<sdbusplus::xyz::openbmc_project::Logging::
                                   server::Create::FFDCFormat,
                               uint8_t, uint8_t, sdbusplus::message::unix_fd>>
            pelCalloutInfo;

        pelCalloutInfo.push_back(
            std::make_tuple(sdbusplus::xyz::openbmc_project::Logging::server::
                                Create::FFDCFormat::JSON,
                            static_cast<uint8_t>(0xCA),
                            static_cast<uint8_t>(0x01), ffdcFile.getFileFD()));

        static constexpr auto bootErrorMessage =
            "org.open_power.PHAL.Error.Boot";
        std::string service =
            util::getService(bus, loggingObjectPath, loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), loggingObjectPath,
                                loggingInterface, "CreateWithFFDCFiles");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(bootErrorMessage, level, additionalData, pelCalloutInfo);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("D-Bus call exception",
                        entry("OBJPATH=%s", loggingObjectPath),
                        entry("INTERFACE=%s", loggingInterface),
                        entry("EXCEPTION=%s", e.what()));

        throw std::runtime_error(
            "Error in invoking D-Bus logging create interface");
    }
    catch (std::exception& e)
    {
        throw e;
    }
}

void createPEL(const std::string& event)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));

    try
    {
        std::string service =
            util::getService(bus, loggingObjectPath, loggingInterface);
        auto method = bus.new_method_call(service.c_str(), loggingObjectPath,
                                          loggingInterface, "Create");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(event, level, additionalData);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>(fmt::format("sdbusplus D-Bus call exception",
                                    "OBJPATH={}, INTERFACE={}, EXCEPTION={}",
                                    loggingObjectPath, loggingInterface,
                                    e.what())
                            .c_str());
        ;

        throw std::runtime_error(
            "Error in invoking D-Bus logging create interface");
    }
    catch (std::exception& e)
    {
        log<level::ERR>(
            fmt::format("D-bus call exception", "EXCEPTION={}", e.what())
                .c_str());
        throw e;
    }
}

FFDCFile::FFDCFile(const json& pHALCalloutData) :
    calloutData(pHALCalloutData.dump()),
    calloutFile("/tmp/phalPELCalloutsJson.XXXXXX"), fileFD(-1)
{
    prepareFFDCFile();
}

FFDCFile::~FFDCFile()
{
    removeCalloutFile();
}

int FFDCFile::getFileFD() const
{
    return fileFD;
}

void FFDCFile::prepareFFDCFile()
{
    createCalloutFile();
    writeCalloutData();
    setCalloutFileSeekPos();
}

void FFDCFile::createCalloutFile()
{
    fileFD = mkostemp(const_cast<char*>(calloutFile.c_str()), O_RDWR);

    if (fileFD == -1)
    {
        log<level::ERR>(fmt::format("Failed to create phalPELCallouts "
                                    "file({}), errorno({}) and errormsg({})",
                                    calloutFile, errno, strerror(errno))
                            .c_str());
        throw std::runtime_error("Failed to create phalPELCallouts file");
    }
}

void FFDCFile::writeCalloutData()
{
    ssize_t rc = write(fileFD, calloutData.c_str(), calloutData.size());

    if (rc == -1)
    {
        log<level::ERR>(fmt::format("Failed to write phaPELCallout info "
                                    "in file({}), errorno({}), errormsg({})",
                                    calloutFile, errno, strerror(errno))
                            .c_str());
        throw std::runtime_error("Failed to write phalPELCallouts info");
    }
    else if (rc != static_cast<ssize_t>(calloutData.size()))
    {
        log<level::WARNING>(fmt::format("Could not write all phal callout "
                                        "info in file({}), written byte({}) "
                                        "and total byte({})",
                                        calloutFile, rc, calloutData.size())
                                .c_str());
    }
}

void FFDCFile::setCalloutFileSeekPos()
{
    int rc = lseek(fileFD, 0, SEEK_SET);

    if (rc == -1)
    {
        log<level::ERR>(fmt::format("Failed to set SEEK_SET for "
                                    "phalPELCallouts in file({}), errorno({}) "
                                    "and errormsg({})",
                                    calloutFile, errno, strerror(errno))
                            .c_str());
        throw std::runtime_error(
            "Failed to set SEEK_SET for phalPELCallouts file");
    }
}

void FFDCFile::removeCalloutFile()
{
    close(fileFD);
    std::remove(calloutFile.c_str());
}

} // namespace pel
} // namespace openpower
