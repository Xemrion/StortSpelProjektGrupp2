#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include "defs.hpp"

namespace config
{
   F32  constexpr  TILE_SIDE_LENGTH         =  30.0f; // rec: 20
   U8   constexpr  CELL_SIZE                =    16;
   U8   constexpr  MIN_TILES_BEFORE_TURN    =     3;
   U8   constexpr  MIN_TILES_BEFORE_BRANCH  =     3;
   U8   constexpr  STEP_SIZE                =     1;
}
