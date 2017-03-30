#include <fstream>
#include <sstream>
#include <iostream>
#include "cfam_access.hpp"
#include "p9_cfam.hpp"
#include "registration.hpp"
#include "targeting.hpp"

namespace openpower
{
namespace p9
{

using namespace openpower::cfam::access;
using namespace openpower::targeting;
using namespace openpower::util;

void CFAMOverride() {
    cfam_address_t address = 0;
    cfam_data_t data = 0;
    cfam_mask_t mask = 0;

    Targeting targets;
    const auto& master = *(targets.begin());

    std::string line;

    std::ifstream overrides("/etc/cfam_overrides");

    if (overrides.is_open())
    {
        while(std::getline(overrides,line))
        {
            if(!line.empty())
            {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                if(!line.empty() && line.at(0) != '#')
                {
                    mask = 0xFFFFFFFF;
                    sscanf(line.c_str(), "%hx %x %x", &address, &data, &mask);
                    writeRegWithMask(master, address, data, mask);
                }
            }
        }
        overrides.close();
    }

    return;
}

REGISTER_PROCEDURE("CFAMOverride", CFAMOverride);

}
}
