#pragma once

#include "Map.hpp"
#include "defs.hpp"
#include "Tile.hpp"

String  createMinimapTexture(  Map const &, Bool isDistrictColoured=false );
String  createFogOfWarTexture( Map const & );
String  createDistanceTexture( Map const & );