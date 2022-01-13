#pragma once

#include <optional>
#include <string>
#include <vector>

namespace openpower::log
{
/**
 * Gather messages from the journal for the current program
 *
 * Fetch the specified number of messages from the journal entry for the
 * currently executed program.
 *
 * @param   executable - Name of the executable
 * @param   maxMessages - Maximum number of messages fetch
 * @return  Optional vector of journal entry data
 */
std::optional<std::vector<std::string>>
    getJournalLog(const std::string& executable, uint16_t maxMessages);
} // namespace openpower::log
