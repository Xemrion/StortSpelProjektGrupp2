#pragma once

#include <iostream>
#include "defs.hpp"

enum class Tile : U8 
{
   ground,
   road,
   building,
   water
};

#pragma warning( disable : 4715 )
[[deprecated]] inline RGBA constexpr  minimapColorLookUpTable( Tile t ) noexcept {
   switch (t) {
      case Tile::ground:   return 0xFF'15C499;
      case Tile::road:     return 0xFF'666666;
      case Tile::building: return 0xFF'333333;
      case Tile::water:    return 0xFF'4B4B19;
   }
   assert( false && "Unaccounted for enum value!" );
}
