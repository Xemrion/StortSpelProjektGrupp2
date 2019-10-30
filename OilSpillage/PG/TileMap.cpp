#include "TileMap.hpp"

TileMap::TileMap()
{
	this->height = 0;
	this->width = 0;
}

TileMap::TileMap(MapConfig const& config):
	config(config),
	width(static_cast<Size>(config.dimensions.y)),
	height(static_cast<Size>(config.dimensions.x)),
	data(Vector<Tile>(width* height, Tile::ground))
{}

TileMap::~TileMap() {}

// x = current X (may be mutated if successful)
// y = current Y (may be mutated if successful)
// d = direction to walk
// returns true if successful
Bool  TileMap::walk( U16 &x, U16 &y, Direction d, Tile tile )
{
	U16  targetX{ x },
		targetY{ y };
	switch (d) {
	case Direction::north: --targetY; break;
	case Direction::east:  ++targetX; break;
	case Direction::south: ++targetY; break;
	case Direction::west:  --targetX; break;
	}

	if (targetX >= width or targetY >= height)
		return false;

	auto targetIndex = index(targetX, targetY);
	if (data[targetIndex] != Tile::ground)
		return false;
	else {
		data[targetIndex] = tile;
		x = targetX;
		y = targetY;
		return true;
	}
}

Vector<GameObject>  TileMap::loadAsModels( Graphics &graphics ) const
{
	Vector<GameObject> tiles( width * height ); // reserve
	for ( U16 y = 0;  y < height;  ++y ) {
		for ( U16 x = 0;  x < width;  ++x ) {
			String currentTileType = "Roads/Metro/";
			if ( tileAt(x,y) != Tile::road )
				currentTileType += "0000";
			else {
				currentTileType += ((y < height-1) and (tileAt(x,y+1) == Tile::road))? '1' : '0'; // S
				currentTileType += ((x <  width-1) and (tileAt(x+1,y) == Tile::road))? '1' : '0'; // E
				currentTileType += ((y >        0) and (tileAt(x,y-1) == Tile::road))? '1' : '0'; // N
				currentTileType += ((x >        0) and (tileAt(x-1,y) == Tile::road))? '1' : '0'; // W
			}
			auto  &tile        { tiles[ index(x, y)] };
			tile.mesh        = graphics.getMeshPointer( currentTileType.c_str() );
			tile.setMaterial(  graphics.getMaterial(    currentTileType.c_str() ) );
			tile.setScale(Vector3{ 1.0f * config.tileScaleFactor.x,
			                       1.0f * config.tileScaleFactor.y,
			                       1.0f * config.tileScaleFactor.z }); // TODO: scale models instead
			tile.setPosition(convertTilePositionToWorldPosition(x, y) - Vector3{ .0f, 1.5f, .0f } );
		}
	}
	return tiles; // RVO/Copy Elision
/*
   auto const  toReserve { width * height };
	Vector<GameObject> tiles( toReserve );
	for ( U16 y = 0;  y < height;  ++y ) {
		for ( U16 x = 0;  x < width;  ++x ) {
			auto const  tileIndex             { index(x, y)                       };
			auto       &tile                  { tiles[tileIndex]                  };
			auto const  graphicsIndex         { getTileLookupIndex(x, y)          };
			auto const &[modelName, rotation] { tileGraphicsTable[graphicsIndex]  };
			tile.mesh      = graphics.getMeshPointer(    modelName.c_str()        );
			tile.setMaterial(graphics.getMaterial(modelName.c_str()));

			if ( rotation != 0 )
				tile.setRotation({ .0f, F32(rotation) * 3.1415926535f/180.0f, .0f });
			tile.setPosition(convertTilePositionToWorldPosition(x, y) - Vector3{ .0f, 1.5f, .0f } );
		}
	}
	return tiles; // RVO/Copy Elision
*/
}

Bool  TileMap::neighbourIsRoad(Direction dir, U16 x, U16 y) const noexcept
{
	assert( dir == Direction::north or dir == Direction::east
	     or dir == Direction::south or dir == Direction::west);

	if      ( dir == Direction::north )    y--;
	else if ( dir == Direction::east  )    x++;
	else if ( dir == Direction::south )    y++;
	else /* ( dir == Direction::west  ) */ x--;

	return isInBounds(x, y) and (data[index(x, y)] == Tile::road);
}

F32  TileMap::getRoadCoverage() const noexcept
{
	Size  roadTileCount{ 0 };
	for (Size i = 0; i < width * height; ++i)
		if (data[i] == Tile::road)
			roadTileCount++;
	return F32(roadTileCount) / F32(width * height);
}


// road generator stream outputter implementation
std::ostream &operator<<( std::ostream &out, TileMap const &map )
{
	if constexpr ( isDebugging )
		out << "  PRINTING " << map.width << 'x' << map.height << " MAP:\n\t";
	for (U16 y = 0; y < map.height; ++y) {
		for (U16 x = 0; x < map.width; ++x) {
			//*[@DEPRECATED]*/  out << ' ' << map.data[map.index( x, y )];
			#ifdef NO_TERMINAL_COLORS
				out << map.tileTerminalColorTable[map.getTileColorLookupIndex(x, y)]
				    << map.tileTerminalGraphicsTable[map.getTileLookupIndex(x, y)]
				    << map.terminalColorDefault;
			#else
				out << map.tileTerminalGraphicsTable[ map.getTileLookupIndex(x, y) ];
			#endif
		}
		out << "\n\t";
	}
	return out;
}

Vector<V2u> TileMap::getCardinallyNeighbouringTilePositions( V2u tilePosition ) const noexcept
{
	Vector<V2u>  neighbouringTilePositions{};
	neighbouringTilePositions.reserve(4);

	// check if any of the neighbouring tile coords are tiles within map bounds:
	Bool const  isBorderingWest { tilePosition.x == 0          },
		isBorderingEast          { tilePosition.x == width  - 1 },
		isBorderingNorth         { tilePosition.y == 0          },
		isBorderingSouth         { tilePosition.y == height - 1 };
	if (not isBorderingNorth) // N
		neighbouringTilePositions.emplace_back( tilePosition.x,     tilePosition.y - 1 );
	if (not isBorderingSouth) // S
		neighbouringTilePositions.emplace_back( tilePosition.x,     tilePosition.y + 1 );
	if (not isBorderingWest) // W
		neighbouringTilePositions.emplace_back( tilePosition.x - 1, tilePosition.y     );
	if (not isBorderingEast) // E
		neighbouringTilePositions.emplace_back( tilePosition.x + 1, tilePosition.y     );
	return neighbouringTilePositions;
}


Vector<V2u> TileMap::getNeighbouringTilePositions( V2u tilePosition ) const noexcept
{
	Vector<V2u>  neighbouringTilePositions {};
	neighbouringTilePositions.reserve(8);
	// check if any of the neighbouring tile coords are tiles within map bounds:
	Bool const  isBorderingWest  = ( tilePosition.x == 0          ),
	            isBorderingEast  = ( tilePosition.x == width  - 1 ),
	            isBorderingNorth = ( tilePosition.y == 0          ),
	            isBorderingSouth = ( tilePosition.y == height - 1 );
	if ( not isBorderingNorth )
	{  // N
		neighbouringTilePositions.emplace_back(    tilePosition.x,   tilePosition.y-1 );
		if ( not isBorderingWest ) // NW
			neighbouringTilePositions.emplace_back( tilePosition.x-1, tilePosition.y-1 );
		if ( not isBorderingEast ) // NE 
			neighbouringTilePositions.emplace_back( tilePosition.x+1, tilePosition.y-1 );
	}
	if ( not isBorderingSouth ) { // S
		neighbouringTilePositions.emplace_back(    tilePosition.x,   tilePosition.y+1 );
		if ( not isBorderingWest ) // SW
			neighbouringTilePositions.emplace_back( tilePosition.x-1, tilePosition.y+1 );
		if ( not isBorderingEast ) // SE 
			neighbouringTilePositions.emplace_back( tilePosition.x+1, tilePosition.y+1 );
	}
	if ( not isBorderingWest )    // W
		 neighbouringTilePositions.emplace_back(   tilePosition.x-1, tilePosition.y );
	if ( not isBorderingEast )    // E
		neighbouringTilePositions.emplace_back(    tilePosition.x+1, tilePosition.y );
	return neighbouringTilePositions;
}


Vector<Size> TileMap::getNeighbouringIndices( V2u tilePosition ) const noexcept
{
	Vector<Size> neighbouringIndex;
	neighbouringIndex.reserve(8);
	// check if any of the neighbouring tile coords are tiles within map bounds:
	Bool const  isBorderingWest  = ( tilePosition.x == 0          ),
	            isBorderingEast  = ( tilePosition.x == width  - 1 ),
	            isBorderingNorth = ( tilePosition.y == 0          ),
	            isBorderingSouth = ( tilePosition.y == height - 1 );
	if ( not isBorderingNorth )
	{  // N
		neighbouringIndex.emplace_back(    index(tilePosition.x,     tilePosition.y - 1));
		if ( not isBorderingWest ) // NW
			neighbouringIndex.emplace_back( index(tilePosition.x - 1, tilePosition.y - 1));
		if ( not isBorderingEast ) // NE 
			neighbouringIndex.emplace_back( index(tilePosition.x + 1, tilePosition.y - 1));
	}
	if ( not isBorderingSouth ) { // S
		neighbouringIndex.emplace_back(    index(tilePosition.x,     tilePosition.y + 1));
		if ( not isBorderingWest ) // SW
			neighbouringIndex.emplace_back( index(tilePosition.x - 1, tilePosition.y + 1));
		if ( not isBorderingEast ) // SE 
			neighbouringIndex.emplace_back( index(tilePosition.x + 1, tilePosition.y + 1));
	}
	if ( not isBorderingWest )    // W
		neighbouringIndex.emplace_back(    index(tilePosition.x - 1, tilePosition.y    ));
	if ( not isBorderingEast )    // E
		neighbouringIndex.emplace_back(    index(tilePosition.x + 1, tilePosition.y    ));
	return neighbouringIndex;
}


// TODO: rotate bend mesh 180 degrees and update values in table to their proper value
Vector<TileMap::TileEntry> const  TileMap::tileGraphicsTable{
	// targetIndex   WSEN      filename          rot      type                  rotation
	/*   0   0000 */  {"Roads/Road_pavement",        0}, // no road,                 0 deg
	/*   1   0001 */  {"Roads/Road_deadend",         0}, // deadend (south),         0 deg
	/*   2   0010 */  {"Roads/Road_deadend",        90}, // deadend (west),         90 deg
	/*   3   0011 */  {"Roads/Road_bend",          180}, // turn,                    0 deg
	/*   4   0100 */  {"Roads/Road_deadend",       180}, // deadend (north),       180 deg
	/*   5   0101 */  {"Roads/Road_straight",        0}, // straight vertical,       0 deg
	/*   6   0110 */  {"Roads/Road_bend",          270}, // turn,                   90 deg
	/*   7   0111 */  {"Roads/Road_3way",           90}, // 3-way intersection,     90 deg
	/*   8   1000 */  {"Roads/Road_deadend",       270}, // deadend (east),        270 deg
	/*   9   1001 */  {"Roads/Road_bend",           90}, // turn,                  270 deg
	/*  10   1010 */  {"Roads/Road_straight",       90}, // straight horizontal,    90 deg
	/*  11   1011 */  {"Roads/Road_3way",            0}, // 3-way intersection,      0 deg
	/*  12   1100 */  {"Roads/Road_bend",            0}, // turn,                  180 deg
	/*  13   1101 */  {"Roads/Road_3way",          270}, // 3-way intersection,    270 deg
	/*  14   1110 */  {"Roads/Road_3way",          180}, // 3-way intersection,    180 deg
	/*  15   1111 */  {"Roads/Road_4way",            0}, // 4-way intersection,      0 deg
};

// Used with a cellular automata to beautify the terminal output.
Vector<String> const   TileMap::tileTerminalGraphicsTable{
	// idx   WSEN     char      type                  rotation
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
Vector<String> const  TileMap::tileTerminalColorTable{
	/* ground */    "\033[38;5;150m",
	/* road0  */    "\033[38;5;255m",
	/* road1  */    "\033[38;5;249m",
	/* road2  */    "\033[38;5;246m",
	/* road3  */    "\033[38;5;243m"
};
