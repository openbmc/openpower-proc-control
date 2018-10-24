#pragma once

namespace openpower
{
namespace cfam
{
namespace p9
{

static constexpr uint32_t P9_DD10_CHIPID           = 0x120D1049;

static constexpr uint16_t P9_FSI_A_SI1S            = 0x081C;
static constexpr uint16_t P9_LL_MODE_REG           = 0x0840;
static constexpr uint16_t P9_FSI2PIB_CHIPID        = 0x100A;
static constexpr uint16_t P9_FSI2PIB_INTERRUPT     = 0x100B;
static constexpr uint16_t P9_FSI2PIB_TRUE_MASK     = 0x100D;
static constexpr uint16_t P9_CBS_CS                = 0x2801;
static constexpr uint16_t P9_SBE_CTRL_STATUS       = 0x2808;
static constexpr uint16_t P9_SBE_MSG_REGISTER      = 0x2809;
static constexpr uint16_t P9_ROOT_CTRL0            = 0x2810;
static constexpr uint16_t P9_PERV_CTRL0            = 0x281A;
static constexpr uint16_t P9_HB_MBX5_REG           = 0x283C;
static constexpr uint16_t P9_SCRATCH_REGISTER_8    = 0x283F;
static constexpr uint16_t P9_ROOT_CTRL8            = 0x2918;
static constexpr uint16_t P9_ROOT_CTRL1_CLEAR      = 0x2931;
}
}
}
