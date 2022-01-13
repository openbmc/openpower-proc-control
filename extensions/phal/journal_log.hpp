#pragma once

#include <optional>
#include <string>
#include <vector>

namespace openpower::log
{
/**
 * @brief Return journal messages for the specified executable
 *
 * @param[in]   executable - Name of the executable
 * @param[in]   maxMessages - Maximum number of messages fetch
 * @return[out]  Optional vector of journal message data
 */
std::optional<std::vector<std::string>>
    getJournalLog(const std::string& executable, uint16_t maxMessages);
} // namespace openpower::log
