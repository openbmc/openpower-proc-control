#pragma once

#include <cstdarg>
namespace openpower
{
namespace pel
{
namespace detail
{

/**
 * @brief Process debug traces
 *
 * Function adds debug traces to the list so that it will be added to the
 * PEL upon failure
 *
 * @param[in] private_data - pointer to private data, unused now
 * @param[in] fmt format for variable list arguments
 * @param[in] ap list of arguements
 */

void processLogTraceCallback(void* private_data, const char* fmt, va_list ap);

/**
 * @brief Process IPL failure/success status
 *
 * Function  resets the debug traces structure if success is received
 * and creates PEL if failure is received as status
 *
 * @param[in] status - IPL execution status
 */
void processIPLErrorCallback(bool status);

/**
 * @brief Reset trace log list
 */
void reset();
} // namespace detail

/**
 * @brief Add callbacks for debug traces and IPL errors
 *
 * This function adds callback for debug traces from PHAL libraries and IPL
 * errors from hardware procedures
 */
void addPHALCallbacks();
} // namespace pel
} // namespace openpower
