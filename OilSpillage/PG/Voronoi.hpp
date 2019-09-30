#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include <functional>
#include "defs.hpp"

class Voronoi {
public:
// tag dispatch
   struct EuclideanDistanceTag {};
   struct ManhattanDistanceTag {};
// ctors
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, EuclideanDistanceTag );
   Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, ManhattanDistanceTag );
// Voronoi noise data member diagram
   Vec<Size>  diagram;
// member constants:
   U8  const  CELL_SIZE;
   U16 const  WIDTH, HEIGHT;
private:
	inline Size noise_index(   U16 x, U16 y ) const noexcept { return Size(x) * WIDTH + Size(y); }
   inline Size diagram_index( U16 x, U16 y ) const noexcept { return Size(x) * CELL_SIZE * WIDTH + Size(y); }
   Vec<V2i>    generate_noise( RNG &rng )    const noexcept;
   Void        generate_diagram( RNG &rng, std::function<Size(V2i const&, V2i const&)> const &distance_f ) noexcept;
};
