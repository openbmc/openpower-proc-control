#pragma once

#include <getopt.h>
#include <map>
#include <string>

namespace open_power
{
namespace boot
{
namespace util
{

struct optstruct
{
    uint8_t start_major; // Starting major step
    uint8_t end_major;   // Ending major step
    uint8_t start_minor; // Starting minor step
    uint8_t end_minor;   // Ending minor step
};

/**
 * @brief Class - Encapsulates parsing command line options and
 *                populating arguments.
 */
class ArgumentParser
{
  public:
    ArgumentParser(int argc, char** argv);
    ArgumentParser() = delete;
    ArgumentParser(const ArgumentParser&) = delete;
    ArgumentParser(ArgumentParser&&) = default;
    ArgumentParser& operator=(const ArgumentParser&) = delete;
    ArgumentParser& operator=(ArgumentParser&&) = default;
    ~ArgumentParser() = default;
    const std::string& operator[](const std::string& opt);

    static void usage(char** argv);

    static const std::string true_string;
    static const std::string empty_string;

  private:
    std::map<const std::string, std::string> arguments;

    static const option options[];
    static const char* optionstr;
};

/**
 * @brie parse options to a structure
 */
void parseArguments(int argc, char** argv, optstruct& opt);

} // namespace util
} // namespace boot
} // namespace open_power
