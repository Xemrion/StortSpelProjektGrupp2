#pragma once

#include <iostream>
#include <cassert>
#include <DirectXMath.h>
#include "defs.hpp"
#include "MapConfig.hpp"
#include "utils.hpp"
#include "Tile.hpp"
#include "Direction.hpp"
#include "../graphics.h"
#include "../GameObject.h"

// TileMap Generator class
class TileMap {
public:
	// look-up table used with a cellular automata to convert the map data to 3D tiles
	struct TileEntry { String filename = "N/A";  I32 rotation = -1; };

	// used with a cellular automata to beautify the terminal output
	static Vector<TileEntry> const  tileGraphicsTable;
	static Vector<String> const     tileTerminalGraphicsTable;
	static Vector<String> const     tileTerminalColorTable;
	static char constexpr           terminalColorDefault[6] { "\033[0m" };
	Size                            width,height;
	MapConfig const                 config;
	Vector<Tile>                    data;
	TileMap();
	TileMap( MapConfig const & );
	~TileMap();
	F32                 getRoadCoverage() const noexcept;
	Bool                walk( U16 &tileX, U16 &tileY, Direction, Tile ); // side-effects: mutates tileX/tileY if walk successful
	Bool                neighbourIsRoad( Direction, U16 tileX, U16 tileY ) const noexcept; // pretends that all out-of-bounds tiles are roads
	Vector<V2u>         getCardinallyNeighbouringTilePositions( V2u tilePosition ) const noexcept;
	Vector<Size>        getNeighbouringIndices( V2u tilePosition ) const noexcept;	
	Vector<V2u>         getNeighbouringTilePositions( V2u tilePosition ) const noexcept;
	Vector<GameObject>  loadAsModels( Graphics & ) const;

	// returns the proper look-up index for the tile @ x,y in the graphics table
	inline Size  getTileLookupIndex( U16 tileX, U16 tileY ) const noexcept
	{
		return data[index(tileX, tileY)] == Tile::ground ? 0 : // else:
			( (neighbourIsRoad( Direction::north, tileX, tileY ) ? util::pow<Size>(2, 0) : 0 )
			+ (neighbourIsRoad( Direction::east,  tileX, tileY ) ? util::pow<Size>(2, 1) : 0 )
			+ (neighbourIsRoad( Direction::south, tileX, tileY ) ? util::pow<Size>(2, 2) : 0 )
			+ (neighbourIsRoad( Direction::west,  tileX, tileY ) ? util::pow<Size>(2, 3) : 0 ) );
	}

	inline Size  getTileColorLookupIndex( U16 tileX, U16 tileY ) const noexcept
	{
		return static_cast<Size>( data[index(tileX, tileY)] );
	}

	// convert an in-bounds 2D coordinate index ( x, y ) into an 1D index (i)
	inline Size  index( U16 tileX, U16 tileY ) const noexcept
	{
		return static_cast<Size>(tileY) * width + static_cast<Size>(tileX);
	}

	inline Vector3 getWorldPosByIndex( Size index ) const noexcept
	{
		return convertTilePositionToWorldPosition( getTilePosByIndex(index) );
	}

	#pragma warning( disable : 4267 ) 
	inline V2u getTilePosByIndex( Size index ) const noexcept
	{
		return V2u( index % width, index / width );
	}

	// returns true if the x,y coordinate is in-bounds
	inline Bool  isInBounds( U16 tileX, U16 tileY )  const noexcept
	{
		return tileX < width and tileY < height;
	}

	inline Vector3  convertTilePositionToWorldPosition( U16 const tileX, U16 const tileY ) const {
		return { tileX * config.tileScaleFactor.x, 
		         .0f,
		         tileY * -config.tileScaleFactor.y };
	}

	inline Vector3  convertTilePositionToWorldPosition( V2u const &tilePosition ) const
	{
		return convertTilePositionToWorldPosition( tilePosition.x, tilePosition.y );
	}

	inline V2u  convertWorldPositionToTilePosition( Vector3 const &worldPosition ) const
	{
		return { static_cast<U32>( std::floor(worldPosition.x / config.tileScaleFactor.x) ),
		         static_cast<U32>( std::floor(worldPosition.y / config.tileScaleFactor.y) ) };
	}

	//  uses the inputs x and z values
	inline V2u  convertWorldPositionToTilePositionXZ( Vector3 const &worldPosition ) const
	{
		return { static_cast<U32>( std::floor(worldPosition.x / config.tileScaleFactor.x) ),
		         static_cast<U32>( std::floor(worldPosition.z / config.tileScaleFactor.y) ) };
	}

	inline Tile const &tileAt( U16 x, U16 y ) const noexcept
	{
		return data[ index(x, y) ];
	}

	inline Tile const &tileAt( V2u tilePosition ) const noexcept
	{
		return data[ index(tilePosition.x, tilePosition.y) ];
	}

	inline Tile& tileAt( U16 x, U16 y ) noexcept
	{
		return data[ index(x, y) ];
	}

	inline Tile &tileAt( V2u tilePosition ) noexcept
	{
		return data[ index(tilePosition.x, tilePosition.y) ];
	}

	friend std::ostream &operator<< ( std::ostream &, TileMap const & );
};

// road generator stream outputter implementation
std::ostream &operator<<( std::ostream& out, TileMap const &map );
