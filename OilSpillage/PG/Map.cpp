#include "Map.hpp"

// x = current X (may be mutated if successful)
// y = current Y (may be mutated if successful)
// d = direction to walk
// returns true if successful
bool Map::walk(U16& x, U16& y, Dir d, Tile tile) {
	U16  _x = x, _y = y;
	switch (d) {
		case Dir::north: --_y; break;
		case Dir::east:  ++_x; break;
		case Dir::south: ++_y; break;
		case Dir::west:  --_x; break;
	}

	if (_x >= width or _y >= height)
		return false;

	auto idx = index(_x, _y);
	if (data[idx] != Tile::ground)
		return false;
	else {
		data[idx] = tile;
		x = _x;
		y = _y;
		return true;
	}
}

Vec<GameObject> Map::load_as_models(Graphics& graphics) const {
	auto const to_reserve = width * height;
	Vec<GameObject> tiles( to_reserve );
	for (U16 y = 0; y < height; ++y) {
		for (U16 x = 0; x < width; ++x) {
			auto const  tile_i = index(x, y);
			auto       &tile = tiles[tile_i];
			auto const  gfx_i = gfx_tbl_idx(x, y);
			auto const &[filename, rotation] = gfx_tbl[gfx_i];
			tile.mesh = graphics.getMeshPointer(filename.c_str());
			if (rotation != 0)
				tile.setRotation({ 0.0f, float(rotation) * 3.1415926535f/180.0f, 0.0f });
			tile.setPosition(tile_xy_to_world_pos(x,y));
		}
	}
	return tiles; // RVO/Copy Elision
}

Bool Map::neighbour_is_road(Dir dir, U16 x, U16 y) const noexcept {
	assert(dir == Dir::north or dir == Dir::east
		or dir == Dir::south or dir == Dir::west);

	if      (dir == Dir::north)      y--;
	else if (dir == Dir::east)       x++;
	else if (dir == Dir::south)      y++;
	else /* ( dir == Dir::west  ) */ x--;

	return !in_bounds(x, y) or (data[index(x, y)] != Tile::ground);
}

Bool Map::is_road(U16 x, U16 y) const noexcept {
	return data[index(x, y)] != Tile::ground;
}

// road generator stream outputter implementation
std::ostream& operator<< (std::ostream& out, Map const& map) {
#ifdef _DEBUG
	out << "  PRINTING " << map.width << 'x' << map.height << " MAP:\n\t";
#endif
	for (U16 y = 0; y < map.height; ++y) {
		for (U16 x = 0; x < map.width; ++x) {
			//*[@DEPRECATED]*/  out << ' ' << map.data[map.index(x,y)];
			
#ifdef NO_TERMINAL_COLORS
			out << map.term_clr_tbl[map.term_clr_tbl_idx(x, y)]
				<< map.term_gfx_tbl[map.term_gfx_tbl_idx(x, y)]
				<< map.term_clr_def;
#else
			out << map.term_gfx_tbl[map.term_gfx_tbl_idx(x, y)];
#endif
		}

		out << "\n\t";
	}
	return out;
}

// TODO: rotate bend mesh 180 degrees and update values in table to their proper value
Vec<Map::TileEntry> const Map::gfx_tbl = {
	// idx   WSEN      filename          rot      type                  rotation
	/*   0   0000 */  {"Road_pavement.bin",    0}, // no road,                 0 deg
	/*   1   0001 */  {"Road_deadend.bin",     0}, // deadend (south),         0 deg
	/*   2   0010 */  {"Road_deadend.bin",    90}, // deadend (west),         90 deg
	/*   3   0011 */  {"Road_bend.bin",      180}, // turn,                    0 deg
	/*   4   0100 */  {"Road_deadend.bin",   180}, // deadend (north),       180 deg
	/*   5   0101 */  {"Road_straight.bin",    0}, // straight vertical,       0 deg
	/*   6   0110 */  {"Road_bend.bin",      270}, // turn,                   90 deg
	/*   7   0111 */  {"Road_3way.bin",       90}, // 3-way intersection,     90 deg
	/*   8   1000 */  {"Road_deadend.bin",   270}, // deadend (east),        270 deg
	/*   9   1001 */  {"Road_bend.bin",       90}, // turn,                  270 deg
	/*  10   1010 */  {"Road_straight.bin",   90}, // straight horizontal,    90 deg
	/*  11   1011 */  {"Road_3way.bin",        0}, // 3-way intersection,      0 deg
	/*  12   1100 */  {"Road_bend.bin",        0}, // turn,                  180 deg
	/*  13   1101 */  {"Road_3way.bin",      270}, // 3-way intersection,    270 deg
	/*  14   1110 */  {"Road_3way.bin",      180}, // 3-way intersection,    180 deg
	/*  15   1111 */  {"Road_4way.bin",        0}, // 4-way intersection,      0 deg
};

// Used with a cellular automata to beautify the terminal output.
Vec<Str> const Map::term_gfx_tbl = {
	// idx   WSEN     Char      type                  rotation
	/*   0   0000 */  u8".", // no road,                 0 deg
	/*   1   0001 */  u8"╹", // deadend (south),         0 deg
	/*   2   0010 */  u8"╺", // deadend (west),         90 deg
	/*   3   0011 */  u8"╚", // turn,                    0 deg
	/*   4   0100 */  u8"╻", // deadend (north),       180 deg
	/*   5   0101 */  u8"║", // straight vertical,       0 deg
	/*   6   0110 */  u8"╔", // turn,                   90 deg
	/*   7   0111 */  u8"╠", // 3-way intersection,     90 deg
	/*   8   1000 */  u8"╸", // deadend (east),        270 deg
	/*   9   1001 */  u8"╝", // turn,                  270 deg
	/*  10   1010 */  u8"═", // straight horizontal,    90 deg
	/*  11   1011 */  u8"╩", // 3-way intersection,      0 deg
	/*  12   1100 */  u8"╗", // turn,                  180 deg
	/*  13   1101 */  u8"╣", // 3-way intersection,    270 deg
	/*  14   1110 */  u8"╦", // 3-way intersection,    180 deg
	/*  15   1111 */  u8"╬"  // 4-way intersection,      0 deg
};

// Used to color code tiles (for terminal output)
Vec<Str> const Map::term_clr_tbl = {
	/* ground */    "\033[38;5;150m",
	/* road0  */    "\033[38;5;255m",
	/* road1  */    "\033[38;5;249m",
	/* road2  */    "\033[38;5;246m",
	/* road3  */    "\033[38;5;243m"
};
