#pragma once

#include <functional>
#include "TileMap.hpp"
#include "defs.hpp"

class Voronoi {
public:
// tag dispatch
   struct EuclideanDistanceTag {};
   struct ManhattanDistanceTag {}; 

                Voronoi( RNG &, U8 cellSize, U32 width, U32 height, EuclideanDistanceTag );
                Voronoi( RNG &, U8 cellSize, U32 width, U32 height, ManhattanDistanceTag );
	inline Size  noiseIndex(   U32 x, U32 y ) const noexcept { return Size(y) * width + x; }
	inline Size  noiseIndex(   V2u position ) const noexcept { return noiseIndex(position.x, position.y);   }
	inline Size  diagramIndex( U32 x, U32 y ) const noexcept { return Size(y) * width * U32(cellSize) + x;  }
	inline Size  diagramIndex( V2u position ) const noexcept { return diagramIndex(position.x, position.y); }
	Size         computeCellRealEstateArea( U32 const cellId, TileMap const & ) const noexcept;
	F32          computeCellRoadCoverage( U32 const cellId, TileMap const & ) const noexcept;
	Bounds       computeCellBounds( U32 const cellId ) const noexcept;

	U8  const          cellSize;
	U32 const          width,
	                   height;
	Vector<Size>       diagram;
	Vector<V2f> const  noise;

 private:
   Vector<V2f>  generateNoise( RNG & ) const noexcept;
   void         generateDiagram( RNG &, std::function<F32(V2f const&, V2f const&)> const &distanceFunction ) noexcept;
};
