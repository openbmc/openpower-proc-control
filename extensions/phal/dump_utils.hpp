#include <cstdint>

#pragma once

namespace openpower::phal::dump
{

/** @brief Dump types supported by dump request */
enum class DumpType
{
    SBE
};

/** @brief Structure for dump request parameters */
struct DumpParameters
{
    uint32_t logId;
    uint32_t unitId;
    DumpType dumpType;
};

/**
 * Request a dump from the dump manager
 *
 * Request a dump from the dump manager and register a monitor for observing
 * the dump progress.
 *
 * @param dumpParameters Parameters for the dump request
 */
void requestDump(const DumpParameters& dumpParameters);

} // namespace openpower::phal::dump
