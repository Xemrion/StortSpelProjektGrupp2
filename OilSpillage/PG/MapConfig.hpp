#pragma once

#include "defs.hpp"
#include <sstream>

struct MapConfig {
   I32   seed                           {  69469 };
   I32   distanceMapSearchRadius        {      3 };
   V2i   dimensions                     { 48, 48 };
   I32   districtCellSide               {     16 };
   Bool  isUsingManhattanDistance       {   true };
   I32   roadStepSize                   {      1 };
   I32   roadMinTilesBeforeTurn         {      0 }; // factorize?
   I32   roadMinTilesBeforeBranch       {      1 }; // factorize?
   I32   roadDepthMax                   {      4 };
   F32   roadMinTotalCoverage           {   .15f }; // TODO: add to imgui window
   I32   roadLengthMin                  {    150 };
   I32   roadLengthMax                  {    400 };
   F32   roadLengthFactor               {   .80f };
   F32   roadTurnProbability            {   .20f };
   F32   roadTurnProbabilityFactor      {  2.00f };
   F32   roadBranchProbability          {   .20f };
   F32   roadBranchProbabilityFactor    {   .85f };
   F32   districtColorCodingBlendFactor {   .50f };
   F32   buildingFloorHeightFactor      {   .20f };
   V3f   tileScaleFactor                { 20.0f, 20.0f, 20.0f }; 
};

String  mapConfigToFilename( MapConfig const &, String extension = "" );
