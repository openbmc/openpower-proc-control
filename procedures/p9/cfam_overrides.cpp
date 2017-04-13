#include <fstream>
#include <sstream>
#include <iostream>
#include "cfam_access.hpp"
#include "p9_cfam.hpp"
#include "registration.hpp"
#include "targeting.hpp"

/* File /var/lib/obmc/cfam_overrides requires whitespace-separated parameters
Pos Address Data Mask with one register write per line. For example:
0 0x283F 0x12345678 0xF0F0F0F0
0 0x283F 0x87654321 0x0F0F0F0F
Blank lines and comment lines beginning with # will be ignored. */

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

    std::ifstream overrides("/var/lib/obmc/cfam_overrides");

    if (overrides.is_open())
    {
        while (std::getline(overrides,line))
        {
            if (!line.empty())
            {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                if (!line.empty() && line.at(0) != '#')
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
