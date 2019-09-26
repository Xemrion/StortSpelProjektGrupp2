#pragma once

#include <iostream>
#include "defs.hpp"

// tile enums
// road0~4 just encode the depth at which the road was generated in order to facilitate debug output
// TODO: unify as "road" or specific road variants
enum class Tile : U8   { ground, road0, road1, road2, road3, road4 };


// TODO: assert bounds (max level currently is 4)
inline Tile road_tile_at_depth(U8 depth) {
	return static_cast<Tile>(depth+1);
}

// reason for deprecation:  stream output maps instead of individual tiles
[[deprecated]] inline std::ostream& operator<< (std::ostream& out, Tile t) {
	switch (t) {
	   case Tile::ground: out << '.'; break;
	   case Tile::road0:  out << '#'; break;
	   case Tile::road1:  out << '#'; break;
	   case Tile::road2:  out << '#'; break;
	   case Tile::road3:  out << '#'; break;
	   case Tile::road4:  out << '#'; break;
	}
	return out;
}
