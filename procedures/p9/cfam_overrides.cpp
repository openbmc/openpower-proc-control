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
    int pos = 0;
    cfam_address_t address = 0;
    cfam_data_t data = 0;
    cfam_mask_t mask = 0;

    Targeting targets;

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
                    if (sscanf(line.c_str(), "%x %hx %x %x", &pos, &address,
                        &data, &mask) >= 3)
                    {
                        const auto& target = targets.getTarget(pos);
                        writeRegWithMask(target, address, data, mask);
                    }
                    else
                    {
                        throw std::runtime_error("Cannot write to register - "
                                "not enough parameters given.");
                    }
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
