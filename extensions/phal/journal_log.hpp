#pragma once

#include <optional>
#include <string>
#include <vector>

namespace openpower::log
{
/**
 * Return journal messages for the specified executable
 *
 * @param   executable - Name of the executable
 * @param   maxMessages - Maximum number of messages fetch
 * @return  Optional vector of journal message data
 */
std::optional<std::vector<std::string>>
    getJournalLog(const std::string& executable, uint16_t maxMessages);
} // namespace openpower::log
