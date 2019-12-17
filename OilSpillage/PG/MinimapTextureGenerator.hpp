#pragma once

#include "Map.hpp"
#include "defs.hpp"
#include "Tile.hpp"

String  createMinimapTexture(  Map const &, Bool isDistrictColoured=true, Opt<String> customPath={} );
String  createFogOfWarTexture( Map const & );
String  createDistanceTexture( Map const & );