#pragma once

#include <iostream>
#include <algorithm>
#include "defs.hpp"

enum class Tile : U8 
{
   ground,
   inaccessibleGround,
   road,
   building,
   water
};

struct Lot {
	V2u          nw; // upper left corner tile space coordinate
	Vector<Bool> layout;
	U32          width, length;
	inline bool intersects( U32 xT, U32 yT ) const noexcept {
		U32  xL = xT - nw.x,	// lot-space x
           yL = yT - nw.y; // lot-space y
		assert( xL < width and yL < length );
		return layout[yL * width + xL];
	}
	inline auto getCoverage() const noexcept {
		return std::count( layout.begin(), layout.end(), true );
	}
};

#pragma warning( disable : 4715 )
[[deprecated]] inline RGBA constexpr  minimapColorLookUpTable( Tile t ) noexcept {
   switch (t) {
      case Tile::ground:   return 0xFF'15C499;
      case Tile::road:     return 0xFF'666666;
      case Tile::building: return 0xFF'333333;
      case Tile::water:    return 0xFF'4B4B19;
   }
   assert( false and "Unaccounted for enum value!" );
}
