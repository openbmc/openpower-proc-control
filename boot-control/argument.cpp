#include "argument.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
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
    std::cerr << "    --step            Execute boot steps\n";
    std::cerr << "                      separate range of steps\n";
    std::cerr << "                      by '..'\n";
    std::cerr << std::flush;
}

const option ArgumentParser::options[] = {
    {"major", required_argument, nullptr, 'm'},
    {"minor", required_argument, nullptr, 'i'},
    {"step", required_argument, nullptr, 's'},
    {"help", no_argument, nullptr, 'h'},
    {0, 0, 0, 0},
};

const char* ArgumentParser::optionstr = "mish?";

const std::string ArgumentParser::true_string = "true";
const std::string ArgumentParser::empty_string = "";

void parseArguments(int argc, char** argv, optstruct& opt)
{
    bool optionsValid = false;
    auto options = ArgumentParser(argc, argv);

    auto major = (options)["major"];
    auto minor = (options)["minor"];
    auto steps = (options)["step"];

    if ((!minor.empty()) && (!major.empty()))
    {
        opt.start_major = std::stoi(major);
        opt.end_major = opt.start_major;
        opt.start_minor = std::stoi(minor);
        opt.end_minor = opt.start_minor;
        opt.singleStep = true;
        optionsValid = true;
    }
    else if (!steps.empty())
    {
        auto splitPos = steps.find("..");
        if (splitPos == std::string::npos)
        {
            opt.start_major = std::stoi(steps);
            opt.end_major = std::stoi(steps);
            opt.start_minor = 0xFF;
            opt.end_minor = 0xFF;
        }
        else
        {
            opt.start_major = std::stoi(steps.substr(0, splitPos));
            opt.end_major = std::stoi(steps.substr(splitPos + 2));
            opt.start_minor = 0xFF;
            opt.end_minor = 0xFF;
        }
        opt.singleStep = false;
        optionsValid = true;
    }

    if (!optionsValid)
    {
        ArgumentParser::usage(argv);
        exit(-1);
    }
}

} // namespace util
} // namespace boot
} // namespace open_power
