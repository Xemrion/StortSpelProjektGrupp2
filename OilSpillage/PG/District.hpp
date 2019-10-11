#pragma once

#include <cassert>
#include "defs.hpp"

enum class District : U8 { residential,
                           park,
                           metropolitan,
                           suburban,
                           downtown,
                           // ^-- insert new types here
                           size // <-- must always be last
                         };

F32 District_getBuildingDensity( District ) noexcept;
U8  District_getBuildingMinArea( District ) noexcept;
U8  District_getBuildingMaxArea( District ) noexcept;
U8  District_getMinFloorCount(   District ) noexcept;
U8  District_getMaxFloorCount(   District ) noexcept;
Str stringify(                   District ) noexcept;
