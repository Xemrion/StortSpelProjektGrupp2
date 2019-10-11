#pragma once

#include "defs.hpp"
#include "Map.hpp"

// -1 seed => random seed
[[deprecated]]
inline void generateRandomSegments( TileMap *map, U16 numRoads, I32 seed ) {
	RD        rd;
	RNG       rng                ( rd()               );
	U16_Dist  generateX          ( 0, map->width  / 2 );
	U16_Dist  generateY          ( 0, map->height / 2 );
	F32_Dist  generateSelection  ( 0,             100 );

	if ( seed != -1 )
		rng.seed( seed );

	while ( --numRoads ) {
		if (generateSelection(rng) < 50) { // odds: 50-50 vertical/horizontal
			Size  x      = generateX(rng) * 2,
				   yStart = generateY(rng) * 2,
				   yEnd   = generateY(rng) * 2;
			for ( Size y = yStart;  y < yEnd;  ++y )
				map->data[map->index( x, y )] = Tile::road;
		}
		else {
			Size  y      = generateY(rng) * 2,
				   xStart = generateX(rng) * 2,
				   xEnd   = generateX(rng) * 2;
			for ( Size x = xStart;  x < xEnd; ++x )
				map->data[map->index( x, y )] = Tile::road;
		}
	}
}
