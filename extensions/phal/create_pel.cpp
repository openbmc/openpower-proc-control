#include "create_pel.hpp"

#include "extensions/phal/journal_log.hpp"
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
namespace pel
{
using ::phosphor::logging::level;
using ::phosphor::logging::log;

constexpr auto maxJournalTraces = 5;
constexpr static const char* executable = "openpower-proc-control";

constexpr auto loggingObjectPath = "/xyz/openbmc_project/logging";
constexpr auto loggingInterface = "xyz.openbmc_project.Logging.Create";
constexpr auto opLoggingInterface = "org.open_power.Logging.PEL";

namespace internal
{
/**
 * @brief Helper function to fetch the journal traces and append it to FFDC
 * @param[out] additionalData map to which traces needs to be appended
 */
void appendJoundalData(std::map<std::string, std::string>& additionalData)
{
    std::optional<std::vector<std::string>> traces =
        openpower::log::getJournalLog(executable, maxJournalTraces);
    if (traces.has_value())
    {
        int count = 0;
        for (auto i = traces.value().crbegin(); i != traces.value().crend();
             i++)
        {
            std::string log = "Log" + std::to_string(count++);
            additionalData.emplace(log, *i);
        }
    }
}
} // namespace internal

void createErrorPEL(const std::string& event, const json& calloutData,
                    const FFDCData& ffdcData)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();
    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }
    // add last few journal traces to additional data
    internal::appendJoundalData(additionalData);

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

        std::string service =
            util::getService(bus, loggingObjectPath, loggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), loggingObjectPath,
                                loggingInterface, "CreateWithFFDCFiles");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                sdbusplus::xyz::openbmc_project::Logging::server::Entry::Level::
                    Error);
        method.append(event, level, additionalData, pelCalloutInfo);
        auto resp = bus.call(method);
    }
    catch (const sdbusplus::exception::exception& e)
    {
        log<level::ERR>(
            fmt::format("D-Bus call exception",
                        "OBJPATH={}, INTERFACE={}, event={}, EXCEPTION={}",
                        loggingObjectPath, loggingInterface, event, e.what())
                .c_str());
        throw std::runtime_error(
            "Error in invoking D-Bus logging create interface");
    }
    catch (const std::exception& e)
    {
        throw e;
    }
}

uint32_t createSbeErrorPEL(const std::string& event, const sbeError_t& sbeError,
                           const FFDCData& ffdcData, const Severity severity)
{
    uint32_t plid = 0;
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();

    additionalData.emplace("_PID", std::to_string(getpid()));
    additionalData.emplace("SBE_ERR_MSG", sbeError.what());

    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }
    // add last few journal traces to additional data
    internal::appendJoundalData(additionalData);

    std::vector<std::tuple<
        sdbusplus::xyz::openbmc_project::Logging::server::Create::FFDCFormat,
        uint8_t, uint8_t, sdbusplus::message::unix_fd>>
        pelFFDCInfo;

    // get SBE ffdc file descriptor
    auto fd = sbeError.getFd();

    // Negative fd value indicates error case or invalid file
    // No need of special processing , just log error with additional ffdc.
    if (fd > 0)
    {
        // Refer phosphor-logging/extensions/openpower-pels/README.md section
        // "Self Boot Engine(SBE) First Failure Data Capture(FFDC) Support"
        // for details of related to createPEL with SBE FFDC information
        // usin g CreateWithFFDCFiles api.
        pelFFDCInfo.push_back(
            std::make_tuple(sdbusplus::xyz::openbmc_project::Logging::server::
                                Create::FFDCFormat::Custom,
                            static_cast<uint8_t>(0xCB),
                            static_cast<uint8_t>(0x01), sbeError.getFd()));
    }
    try
    {
        std::string service =
            util::getService(bus, loggingObjectPath, opLoggingInterface);
        auto method =
            bus.new_method_call(service.c_str(), loggingObjectPath,
                                opLoggingInterface, "CreatePELWithFFDCFiles");
        auto level =
            sdbusplus::xyz::openbmc_project::Logging::server::convertForMessage(
                severity);
        method.append(event, level, additionalData, pelFFDCInfo);
        auto response = bus.call(method);

        // reply will be tuple containing bmc log id, platform log id
        std::tuple<uint32_t, uint32_t> reply = {0, 0};

        // parse dbus response into reply
        response.read(reply);
        plid = std::get<1>(reply); // platform log id is tuple "second"
    }
    catch (const sdbusplus::exception::exception& e)
    {
        log<level::ERR>(fmt::format("D-Bus call exception",
                                    "OBJPATH={}, INTERFACE={}, EXCEPTION={}",
                                    loggingObjectPath, loggingInterface,
                                    e.what())
                            .c_str());
        throw std::runtime_error(
            "Error in invoking D-Bus logging create interface");
    }
    catch (const std::exception& e)
    {
        throw e;
    }

    return plid;
}

void createPEL(const std::string& event, const FFDCData& ffdcData)
{
    std::map<std::string, std::string> additionalData;
    auto bus = sdbusplus::bus::new_default();

    additionalData.emplace("_PID", std::to_string(getpid()));
    for (auto& data : ffdcData)
    {
        additionalData.emplace(data);
    }
    // add last few journal traces to additional data
    internal::appendJoundalData(additionalData);

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
    catch (const sdbusplus::exception::exception& e)
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
    catch (const std::exception& e)
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
