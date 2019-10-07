#pragma once

#include <functional>
#include "Map.hpp"
#include "defs.hpp"

class Voronoi {
public:
// tag dispatch
   struct EuclideanDistanceTag {};
   struct ManhattanDistanceTag {}; 
// cell bounds
   struct Bounds { V2u min, max; };
// ctors
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, EuclideanDistanceTag );
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, ManhattanDistanceTag );
// member constants:
   U8  const  CELL_SIZE;
   U16 const  WIDTH, HEIGHT;
// Voronoi noise data member diagram
   Vec<Size>      diagram;
   Vec<V2f> const noise;
	inline Size  noise_index(   U16 x, U16 y ) const noexcept { return Size(y) * WIDTH + x; }
   inline Size  diagram_index( U16 x, U16 y ) const noexcept { return Size(y) * WIDTH * U16(CELL_SIZE) + x; }
   Size         compute_cell_real_estate_area( U16 const cell_id, Map const & ) const noexcept;
   Bounds       compute_cell_bounds( U16 const cell_id ) const noexcept;
 private:
   Vec<V2f>     generate_noise( RNG &rng ) const noexcept;
   Void         generate_diagram( RNG &rng, std::function<F32(V2f const&, V2f const&)> const &distance_f ) noexcept;
};

