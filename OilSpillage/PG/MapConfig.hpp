#pragma once

#include "defs.hpp"
#include <sstream>

struct MapConfig {
   I32   seed                           { 42073  };
   V2i   dimensions                     { 96, 96 };

   I32   districtCellSide               {    16 };
   Bool  isUsingManhattanDistance       {  true };

   I32   roadStepSize                   {     1 };
   I32   roadMinTilesBeforeTurn         {     3 };
   I32   roadMinTilesBeforeBranch       {     3 };
   I32   roadDepthMax                   {     3 };
   I32   roadLengthMin                  {    40 };
   I32   roadLengthMax                  {   120 };
   F32   roadLengthFactor               {   .8f };
   F32   roadTurnProbability            {   .1f };
   F32   roadTurnProbabilityFactor      {  1.5f };
   F32   roadBranchProbability          {   .1f };
   F32   roadBranchProbabilityFactor    {  1.5f };

   F32   districtColorCodingBlendFactor {   .5f };
   V3f   tileScaleFactor                { 20.0f, 20.0f, 20.0f }; 
   F32   buildingFloorHeightFactor      {  .20f };
};

Str mapConfigToFilename( MapConfig const &, Str extension = "" );