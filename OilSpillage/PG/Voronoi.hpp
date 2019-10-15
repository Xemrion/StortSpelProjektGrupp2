#pragma once

#include <functional>
#include "TileMap.hpp"
#include "defs.hpp"

class Voronoi {
public:
   struct Bounds { V2u min, max; };
// tag dispatch
   struct EuclideanDistanceTag {};
   struct ManhattanDistanceTag {}; 

                Voronoi( RNG &, U8 cellSize, U16 width, U16 height, EuclideanDistanceTag );
                Voronoi( RNG &, U8 cellSize, U16 width, U16 height, ManhattanDistanceTag );
	inline Size  noiseIndex(   U16 x, U16 y ) const noexcept { return Size(y) * width + x; }
   inline Size  noiseIndex(   V2u position ) const noexcept { return noiseIndex(position.x, position.y);   }
   inline Size  diagramIndex( U16 x, U16 y ) const noexcept { return Size(y) * width * U16(cellSize) + x;  }
   inline Size  diagramIndex( V2u position ) const noexcept { return diagramIndex(position.x, position.y); }
   Size         computeCellRealEstateArea( U16 const cellId, TileMap const & ) const noexcept;
   Bounds       computeCellBounds( U16 const cellId ) const noexcept;
   U8  const       cellSize;
   U16 const       width,
                   height;
   Vector<Size>       diagram;
   Vector<V2f> const  noise;

 private:
   Vector<V2f>     generateNoise( RNG & ) const noexcept;
   void         generateDiagram( RNG &, std::function<F32(V2f const&, V2f const&)> const &distanceFunction ) noexcept;
};
