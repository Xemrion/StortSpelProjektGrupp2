// author: Victor Falkengaard Itzel
// copyright September 2019

#include <cmath>
#include "Voronoi.hpp"

Voronoi::Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, EuclideanDistanceTag ):
   CELL_SIZE(cell_size), WIDTH(width), HEIGHT(height)
{
      // distance function (NOTE! the result is squared but this isn't an issue!)
      auto euclidean_distance = []( V2i const &a, V2i const &b ) -> Size {
	      return std::pow(std::abs(a.x - b.x),2) + std::pow(std::abs(a.y - b.y),2);
      };
      // generate Voronoi diagram with Manhattan distance
      generate_diagram( rng, euclidean_distance );
}

Voronoi::Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, ManhattanDistanceTag ):
   CELL_SIZE(cell_size), WIDTH(width), HEIGHT(height)
{
   // distance function
   auto manhattan_distance = []( V2i const &a, V2i const &b ) -> Size {
	   return std::abs(a.x - b.x) + std::abs(a.y - b.y);
   };
   // generate Voronoi diagram with Manhattan distance
   generate_diagram( rng, manhattan_distance );
}

Vec<V2i> Voronoi::generate_noise( RNG &rng ) const noexcept {
   auto      noise = I32_Dist { 0, CELL_SIZE };
   Vec<V2i>  uniform_noise;
   uniform_noise.reserve( WIDTH*HEIGHT );
   for ( Size x=0;  x<WIDTH;  ++x )
      for ( Size y=0;  y<HEIGHT;  ++y )
   	   uniform_noise[noise_index(x,y)] = { noise(rng)+x, noise(rng)+y };
   return uniform_noise;
}

Void Voronoi::generate_diagram( RNG &rng, std::function<Size(V2i const&, V2i const&)> const &distance_function ) noexcept {
   // generate noise
   Vec<V2i> const noise = generate_noise(rng);

   diagram.reserve( WIDTH*HEIGHT*CELL_SIZE*CELL_SIZE );

   V2i   curr_pos;
   Size  best_match;
   F32   best_distance;
   // computes whether a cell is a better match than the current best
   // mutates best_match and best_distance if a better match is found.
   auto process = [&]( Size const &cell_index ) {
      V2i const &cell_center = noise[cell_index];
      I32        distance    = distance_function( cell_center, curr_pos );
      if ( distance > best_distance ) {
         best_distance = distance;
         best_match    = cell_index;
      }
   };

// main loop
   Size const  DIAGRAM_WIDTH  = CELL_SIZE * WIDTH,
               DIAGRAM_HEIGHT = CELL_SIZE * HEIGHT;
   for ( curr_pos.x=0;  curr_pos.x<DIAGRAM_WIDTH;  ++curr_pos.x ) {
      for ( curr_pos.y=0;  curr_pos.y<DIAGRAM_HEIGHT;  ++curr_pos.y ) {
         Size  cell_x = Size(curr_pos.x/DIAGRAM_WIDTH),
               cell_y = Size(curr_pos.y/DIAGRAM_HEIGHT);
         // start with center cell (current cell):
         process( noise_index(cell_x,cell_y) );
         // check if any of the neighbouring cells are better matches (if eligible):
         Bool const  w_border = (curr_pos.x == 0),
                     e_border = (curr_pos.x == WIDTH),
                     n_border = (curr_pos.y == 0),
                     s_border = (curr_pos.y == HEIGHT);
         if ( !n_border ) {  // N
            process( noise_index(cell_x,cell_y-1) );
            if ( !w_border ) // NW
               process( noise_index(cell_x-1,cell_y-1) );
            if ( !e_border ) // NE 
               process( noise_index(cell_x+1,cell_y-1) );
         }
         if ( !s_border ) {  // S
            process( noise_index(cell_x,cell_y+1) );
            if ( !w_border ) // SW
               process( noise_index(cell_x-1,cell_y+1) );
            if ( !e_border ) // SE 
               process( noise_index(cell_x+2,cell_y+1) );
         }
         if ( !w_border )    // W
            process( noise_index(cell_x-1,cell_y) );
         if ( !e_border )    // E
            process( noise_index(cell_x+2,cell_y) );
         diagram[diagram_index(curr_pos.x,curr_pos.y)] = best_match;
      }
   }
}
