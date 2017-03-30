#include <fstream>
#include <sstream>
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
using namespace std;

void CFAMOverride() {
    cfam_address_t address;
    cfam_data_t data;
    cfam_mask_t mask;

    Targeting targets;
    const auto& master = *(targets.begin());

    string line, segment;
    vector<std::string> segmentList;

    ifstream overrides("/etc/cfam_overrides");

    if (overrides.is_open())
    {
        while(getline(overrides,line))
        {
            stringstream stream(line);
            while(getline(stream, segment, ' '))
            {
                segmentList.push_back(segment);
            }

            address = stoul(segmentList[0], nullptr, 0);
            data = stoul(segmentList[1], nullptr, 0);
            if (segmentList.size() > 2)
            {
                mask = stoul(segmentList[2], nullptr, 0);
            }

            if (segmentList.size() > 2)
            {
                writeRegWithMask(master, address, data, mask);
            }
            else
            {
                writeReg(master, address, data);
            }

            segmentList.clear();
        }
        overrides.close();
    }

    return;
}

REGISTER_PROCEDURE("CFAMOverride", CFAMOverride);

}
}
