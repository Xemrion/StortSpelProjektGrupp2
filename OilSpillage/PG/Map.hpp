#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include <iostream>
#include <cassert>
#include "defs.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "Tile.hpp"
#include "Direction.hpp"
#include "../graphics.h"
#include "../GameObject.h"

// Map Generator class
class Map {
public:
	U16 const  width, height;
	Vec<Tile>  data;

	Map(U16 width, U16 height) :
		width  (width),
		height (height),
		data   ( Vec<Tile>( Size(width)*Size(height), Tile::ground ) )
	{}

	// x = current X (may be mutated if successful)
	// y = current Y (may be mutated if successful)
	// d = direction to walk
	// returns true if successful
	Bool walk(U16& x, U16& y, Dir d, Tile tile);

	// stream outputter friend declaration
	friend std::ostream& operator<< (std::ostream&, Map const&);

	// look-up table used with a cellular automata to convert the map data to 3D tiles.
	struct TileEntry   { Str filename = "N/A";   I32 rotation = -1; };

	static Vec<TileEntry> const gfx_tbl;

	// returns the proper look-up index for the tile @ x,y in the graphics table
	inline Size gfx_tbl_idx(U16 x, U16 y) const noexcept {
		return data[index(x, y)] == Tile::ground ? 0 : // else:
		    ( (neighbour_is_road(Dir::north, x, y) ? cPow(2,0) : 0)
			 + (neighbour_is_road(Dir::east,  x, y) ? cPow(2,1) : 0)
			 + (neighbour_is_road(Dir::south, x, y) ? cPow(2,2) : 0)
			 + (neighbour_is_road(Dir::west,  x, y) ? cPow(2,3) : 0) );
	}

	// Used with a cellular automata to beautify the terminal output.
	static Vec<Str> const term_gfx_tbl;

	// Used to color code tiles (for terminal output)
	static Vec<Str> const term_clr_tbl;

	static Char constexpr term_clr_def[6] = "\033[0m"; // default colour resetter

	inline Size term_clr_tbl_idx(U16 x, U16 y) const noexcept {
		return static_cast<Size>(data[index(x, y)]);
	}

	// returns the proper look-up index for the tile @ x,y in the graphics table
	inline Size term_gfx_tbl_idx(U16 x, U16 y) const noexcept {
		return data[index(x, y)] == Tile::ground ? 0 : // else:
			  (neighbour_is_road(Dir::north, x, y) ? cPow(2,0) : 0)
			+ (neighbour_is_road(Dir::east,  x, y) ? cPow(2,1) : 0)
			+ (neighbour_is_road(Dir::south, x, y) ? cPow(2,2) : 0)
			+ (neighbour_is_road(Dir::west,  x, y) ? cPow(2,3) : 0);
	}

	Vec<GameObject> load_as_models(Graphics& graphics) const;

	// NOTE! pretends that all out-of-bounds tiles are roads
	Bool neighbour_is_road(Dir dir, U16 x, U16 y) const noexcept;

	Bool is_road(U16 x, U16 y) const noexcept;

	// convert an in-bounds 2D coordinate index (x,y) into an 1D index (i)
	inline Size index(U16 x, U16 y) const noexcept {
		return Size(y) * width + Size(x);
	}

	// returns true if the x,y coordinate is in-bounds
	inline Bool in_bounds(U16 x, U16 y) const noexcept {
		return x < width and y < height;
	}

	inline Vector3 tile_xy_to_world_pos(U16 const x, U16 const y) const {
		static auto const x_offset = width  / 2.0f * config::SIDE_LENGTH,
			               y_offset = height / 2.0f * config::SIDE_LENGTH;
		return { x * config::SIDE_LENGTH - x_offset,  .0f,  y * -config::SIDE_LENGTH + y_offset };
	}
};



// road generator stream outputter implementation
std::ostream& operator<< (std::ostream& out, Map const& map);
