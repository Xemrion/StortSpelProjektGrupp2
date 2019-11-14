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

	// convert an in-bounds 2D coordinate index ( x, y ) into an 1D index (i)
	inline Size  index( V2u tileXY ) const noexcept
	{
		return static_cast<Size>(tileXY.y) * width + static_cast<Size>(tileXY.x);
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

	// returns true if the x,y coordinate is in-bounds
	inline Bool  isInBounds(V2u tilePos)  const noexcept
	{
		return tilePos.x < width and tilePos.y < height;
	}

	inline Vector3  convertTilePositionToWorldPosition( U16 const tileX, U16 const tileY ) const {
		Vector3 result { tileX * config.tileScaleFactor.x, 
		                 .0f,
		                 tileY * -config.tileScaleFactor.y };
		//assert( result.x >= .0f );
		//assert( result.z >= .0f );
		return result;
	}

	inline Vector3  convertTilePositionToWorldPosition( V2u const &tilePosition ) const
	{
		return convertTilePositionToWorldPosition( tilePosition.x, tilePosition.y );
	}

	inline V2u  convertWorldPositionToTilePosition( Vector3 const &worldPosition ) const
	{
		
		V2u result { static_cast<U32>( std::round(worldPosition.x /  config.tileScaleFactor.x) ),
		             static_cast<U32>( std::round(worldPosition.z / -config.tileScaleFactor.y) ) };
		//assert( result.x <  width );
		//assert( result.y < height );
		return result;
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

	Opt<Vector3> getRandomTilePositionInRadius( Vector3 origin, F32 radius, Tile target, U16 tries=100 ) const noexcept;

	Vector<Vector3> getAllTilePositionsInRadius( Vector3 origin, F32 radius, Tile target ) const noexcept;

	friend std::ostream &operator<< ( std::ostream &, TileMap const & );

private:
	Bounds calculateBounds( Vector3 const &origin, F32 radius ) const noexcept;
};

// road generator stream outputter implementation
std::ostream &operator<<( std::ostream& out, TileMap const &map );
