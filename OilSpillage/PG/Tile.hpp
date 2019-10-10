#pragma once

#include <iostream>
#include "defs.hpp"

// tile enums
// road0~4 just encode the depth at which the road was generated in order to facilitate debug output
// TODO: unify as "road" or specific road variants
enum class Tile : U8 {
   ground,
   road0,
   road1,
   road2,
   road3,
   road4,
   building,
   water
   // MAKE SURE TO CHECK TileMap::isRoad(x,y) IF ADDING NEW TILE TYPES!!!!
};


// TODO: assert bounds (max level currently is 4)
inline Tile roadTileAtDepth(U8 depth) {
	return static_cast<Tile>(depth+1);
}

using RGBA = U32;

#pragma warning( disable : 4715 )
RGBA constexpr minimapColorLookUpTable( Tile t ) {
   switch (t) {
      case Tile::ground:   return 0xFF'15C499;
      case Tile::road0:
      case Tile::road1: 
      case Tile::road2: 
      case Tile::road3: 
      case Tile::road4:    return 0xFF'666666;
      case Tile::building: return 0xFF'333333;
      case Tile::water:    return 0xFF'4B4B19;
   }
   assert( false && "Unaccounted for enum value!" );
}
