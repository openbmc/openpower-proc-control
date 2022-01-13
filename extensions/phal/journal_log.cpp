#include "extensions/phal/journal_log.hpp"

#include <fmt/format.h>
#include <systemd/sd-journal.h>

#include <phosphor-logging/log.hpp>

#include <memory>

namespace openpower::log
{
using ::phosphor::logging::level;
using ::phosphor::logging::log;
namespace
{
/**
 * @brief Helper method to fetch the value of the specified filed from journal
 * entry
 *
 * @param[in]   journal - journal entry
 * @param[in]   field - field to search for in the journal entry
 * @return  field value
 */
std::string getFieldValue(sd_journal* journal, const std::string_view& field)
{
    const char* data = nullptr;
    size_t length = 0;
    std::string value;
    // Get the metadata from the requested field of the journal entry
    int ret = sd_journal_get_data(
        journal, field.data(), reinterpret_cast<const void**>(&data), &length);
    if (ret < 0)
    {
        log<level::ERR>(
            fmt::format("failed to get journal data ret={}", ret).c_str());
        return value;
    }
    std::string_view contents{data, length};
    // Only use the content after the "=" character.
    contents.remove_prefix(std::min(contents.find('=') + 1, contents.size()));
    value = contents;
    return value;
}
} // namespace

std::optional<std::vector<std::string>>
    getJournalLog(const std::string& executable, uint16_t maxMessages)
{
    sd_journal* journalTmp = nullptr;
    int ret = sd_journal_open(&journalTmp, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0)
    {
        log<level::ERR>(
            fmt::format("failed to open journal ret={}", ret).c_str());
        return std::nullopt;
    }
    std::unique_ptr<sd_journal, decltype(&sd_journal_close)> journal(
        journalTmp, sd_journal_close);

    std::vector<std::string> messages;
    SD_JOURNAL_FOREACH_BACKWARDS(journal.get())
    {
        // Get input field
        std::string syslog = getFieldValue(journal.get(), "SYSLOG_IDENTIFIER");
        if (syslog == executable)
        {
            // Get timestamp
            uint64_t usec{0};
            ret = sd_journal_get_realtime_usec(journal.get(), &usec);
            if (ret == 0)
            {
                // Convert realtime microseconds to date format
                char dateBuffer[80];
                std::string date;
                std::time_t timeInSecs = usec / 1000000;
                strftime(dateBuffer, sizeof(dateBuffer), "%b %d %H:%M:%S",
                         std::localtime(&timeInSecs));
                date = dateBuffer;
                // Store value to messages
                std::string pid = getFieldValue(journal.get(), "_PID");
                std::string message = getFieldValue(journal.get(), "MESSAGE");
                std::string value =
                    date + " " + syslog + "[" + pid + "]: " + message;
                messages.emplace_back(value);
            }
            else
            {
                log<level::ERR>(fmt::format("Failed to get timestamp from "
                                            "journal entry ret={}",
                                            ret)
                                    .c_str());
            }
        } // progname
        // limit maximum number of messages
        if (messages.size() >= maxMessages)
        {
            break;
        }
    } // SD_JOURNAL_FOREACH_BACKWARDS
    if (messages.size() <= 0)
    {
        return std::nullopt;
    }
    return messages;
}
} // namespace openpower::log
