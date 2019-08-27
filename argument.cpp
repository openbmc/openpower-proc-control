#include "argument.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>

namespace open_power
{
namespace boot
{
namespace util
{

ArgumentParser::ArgumentParser(int argc, char** argv)
{
    auto option = 0;
    while (-1 != (option = getopt_long(argc, argv, optionstr, options, NULL)))
    {
        if ((option == '?') || (option == 'h'))
        {
            usage(argv);
            exit(-1);
        }

        auto i = &options[0];
        while ((i->val != option) && (i->val != 0))
        {
            ++i;
        }

        if (i->val)
        {
            arguments[i->name] = (i->has_arg ? optarg : true_string);
        }
    }
}

const std::string& ArgumentParser::operator[](const std::string& opt)
{
    auto i = arguments.find(opt);
    if (i == arguments.end())
    {
        return empty_string;
    }
    else
    {
        return i->second;
    }
}

void ArgumentParser::usage(char** argv)
{
    std::cerr << "Usage: " << argv[0] << " [options]\n";
    std::cerr << "Options:\n";
    std::cerr << "    --help            Print this menu\n";
    std::cerr << "    --major           Step Major Number\n";
    std::cerr << "    --minor           Step Minor Number\n";
    std::cerr << "    --mode            Boot mode\n";
    std::cerr << "                      Valid modes Normal,step\n";
    std::cerr << "    --type            Boot type\n";
    std::cerr << "                      Valid types: on,off,reboot\n";
    std::cerr << std::flush;
}

const option ArgumentParser::options[] = {
    {"major", required_argument, nullptr, 'm'},
    {"minor", required_argument, nullptr, 'i'},
    {"mode", optional_argument, nullptr, 'd'},
    {"type", optional_argument, nullptr, 't'},
    {"help", no_argument, nullptr, 'h'},
    {0, 0, 0, 0},
};

const char* ArgumentParser::optionstr = "midth?";

const std::string ArgumentParser::true_string = "true";
const std::string ArgumentParser::empty_string = "";

} // namespace util
} // namespace boot
} // namespace open_power
