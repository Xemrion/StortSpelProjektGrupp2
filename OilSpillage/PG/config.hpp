#pragma once

#include "defs.hpp"

struct Config {
   // TODO: refactor
   V2i  map_dimensions           { 96, 96 };
   V3f  tile_scale               { 20.0f, 20.0f, 20.0f }; // rec: 20  
   F32  floor_height_factor      {  .20f };
   I32  seed                     { 42073 };
   I32  cell_side                {    16 };
   I32  min_tiles_before_turn    {     3 };
   I32  min_tiles_before_branch  {     3 };
   I32  road_step_size           {     1 };
   F32  district_blend_factor    {   .5f };
};
