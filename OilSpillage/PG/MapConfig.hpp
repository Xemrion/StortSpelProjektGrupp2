#pragma once

#include "defs.hpp"
#include <sstream>

struct MapConfig {
	I32   seed                           {      -1 };
	I32   distanceMapSearchRadius        {       3 };
	V2i   dimensions                     {  48, 48 };
	I32   districtCellSide               {      16 };
	Bool  isUsingManhattanDistance       {    true };
	I32   roadStepSize                   {       1 }; // TODO: refactor in the walk step size and merge probability
	I32   roadMinTilesBeforeTurn         {       0 }; // factorize?
	I32   roadMinTilesBeforeBranch       {       1 }; // factorize?
	I32   roadDepthMax                   {       4 };
	F32   roadMinTotalCoverage           {   .150f }; // TODO: add to imgui window
	F32   roadMaxTotalCoverage           {   .400f }; // TODO: add to imgui window
	F32   roadLengthFactorMin            {   .008f };
	F32   roadLengthFactorMax            {   .040f };
	F32   roadLengthFactor               {   .800f };
	F32   roadTurnProbability            {   .200f };
	F32   roadTurnProbabilityFactor      {  2.000f };
	F32   roadBranchProbability          {   .150f };
	F32   roadBranchProbabilityFactor    {   .750f };
	F32   districtColorCodingBlendFactor {   .500f };
	F32   tileSideScaleFactor            { 20.000f };
	F32	buildingHeightScaleFactor      {  5.000f };
};

[[deprecated]] String  mapConfigToFilename( MapConfig const &, String extension = "" );
