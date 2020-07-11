extern "C" {
#include <libpdbg.h>
}

#include "create_pel.hpp"
#include "phal_error.hpp"

#include <libekb.H>
#include <libipl.H>

#include <iomanip>
#include <phosphor-logging/elog.hpp>
#include <sstream>

namespace openpower
{
namespace pel
{
using namespace phosphor::logging;

namespace detail
{

// keys need to be unique so using counter value to generate unique key
static int counter = 0;

// list of debug traces
static std::vector<std::pair<std::string, std::string>> traceLog;

void processLogTraceCallback(void* private_data, const char* fmt, va_list ap)
{
    va_list vap;
    va_copy(vap, ap);
    std::vector<char> logData(1 + std::vsnprintf(nullptr, 0, fmt, ap));
    std::vsnprintf(logData.data(), logData.size(), fmt, vap);
    va_end(vap);
    std::string logstr(logData.begin(), logData.end());

    log<level::INFO>(logstr.c_str());

    char timeBuf[80];
    time_t t = time(0);
    tm myTm{};
    gmtime_r(&t, &myTm);
    strftime(timeBuf, 80, "%Y-%m-%d %H:%M:%S", &myTm);

    // key values need to be unique for PEL
    // TODO #openbmc/dev/issues/1563
    // If written to Json no need to worry about unique KEY
    std::stringstream str;
    str << std::setfill('0');
    str << "LOG" << std::setw(3) << counter;
    str << " " << timeBuf;
    traceLog.emplace_back(std::make_pair(str.str(), std::move(logstr)));
    counter++;
}

void processBootErrorCallback(bool status)
{
    log<level::INFO>("processBootCallback ", entry("STATUS=%d", status));
    try
    {
        // If failure in hwp execution
        if (!status)
        {
            FFDCData ffdc = libekb_get_ffdc();
            ffdc.insert(ffdc.end(), traceLog.begin(), traceLog.end());
            openpower::pel::createBootErrorPEL(ffdc);
        }
    }
    catch (std::exception& ex)
    {
        reset();
        throw ex;
    }
    reset();
}

void reset()
{
    // reset the trace log and counter
    traceLog.clear();
    counter = 0;
}

void pDBGLogTraceCallbackHelper(int log_level, const char* fmt, va_list ap)
{
    processLogTraceCallback(NULL, fmt, ap);
}
} // namespace detail

static inline int8_t getLogLevelFromEnv( const char* env, const int8_t dValue)
{
    auto logLevel = dValue;
    try
    {
         if(const char* env_p = std::getenv(env))
         {
              logLevel = std::stoi(env_p);
         }
    }
    catch (std::exception& e)
    {
         log<level::ERR>(("Conversion Failure"),
                    entry("ENVIRONMENT=%s", env),
                    entry("EXCEPTION=%s", e.what()));
    }
    return logLevel;
}

void addBootErrorCallbacks()
{
    // Get individual phal repos log level from environment variable.
    auto pdbgLevel = getLogLevelFromEnv("PDBG_LOG", PDBG_INFO) ;
    auto iplLevel = getLogLevelFromEnv("IPL_LOG", IPL_INFO);
    auto libekbLevel = getLogLevelFromEnv("LIBEKB_LOG", LIBEKB_LOG_IMP);

    // set log level to info
    pdbg_set_loglevel(pdbgLevel);
    libekb_set_loglevel(libekbLevel);
    ipl_set_loglevel(iplLevel);

    // add callback for debug traces
    pdbg_set_logfunc(detail::pDBGLogTraceCallbackHelper);
    libekb_set_logfunc(detail::processLogTraceCallback, NULL);
    ipl_set_logfunc(detail::processLogTraceCallback, NULL);

    // add callback for ipl failures
    ipl_set_error_callback_func(detail::processBootErrorCallback);
}
} // namespace pel
} // namespace openpower
