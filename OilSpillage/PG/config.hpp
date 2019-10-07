#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include "defs.hpp"

namespace config
{
   // TODO: refactor
   U32  constexpr  map_width                =    96;
   U32  constexpr  map_height               =    96;
   F32  constexpr  TILE_SIDE_LENGTH         =  3.0f; // rec: 20
   F32  constexpr  TILE_HEIGHT              = TILE_SIDE_LENGTH * 0.1f;
   I32  constexpr  START_SEED               = 42069;
   U8   constexpr  CELL_SIZE                =    16;
   U8   constexpr  MIN_TILES_BEFORE_TURN    =     3;
   U8   constexpr  MIN_TILES_BEFORE_BRANCH  =     3;
   U8   constexpr  STEP_SIZE                =     1;
}
