#include <cmath>
#include <cassert>
#include "Voronoi.hpp"

Voronoi::Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, EuclideanDistanceTag ):
   CELL_SIZE(cell_size),
   WIDTH(width),
   HEIGHT(height),
   diagram( Size(WIDTH)*HEIGHT*CELL_SIZE*CELL_SIZE ),
   noise(generate_noise(rng))
{  // distance function (NOTE! the result is squared but this isn't an issue!)
   auto euclidean_distance = []( V2f const &a, V2f const &b ) -> F32 {
	   return std::sqrt(F32( std::pow(std::abs(a.x - b.x),2) + std::pow(std::abs(a.y - b.y),2) ));
   };
   // generate Voronoi diagram with Manhattan distance
   generate_diagram( rng, euclidean_distance );
}

Voronoi::Voronoi( RNG &rng, U8 cell_size, U16 width, U16 height, ManhattanDistanceTag ):
   CELL_SIZE(cell_size),
   WIDTH(width),
   HEIGHT(height),
   diagram(Size(WIDTH)*HEIGHT*CELL_SIZE*CELL_SIZE),
   noise(generate_noise(rng))
{  // distance function
   auto manhattan_distance = []( V2f const &a, V2f const &b ) -> F32 {
	   return F32( std::abs(a.x - b.x) + std::abs(a.y - b.y) );
   };
   // generate Voronoi diagram with Manhattan distance
   generate_diagram( rng, manhattan_distance );
}

Vec<V2f> Voronoi::generate_noise( RNG &rng ) const noexcept {
   F32_Dist  noise( .0f, CELL_SIZE );
   Vec<V2f>  uniform_noise( Size(WIDTH)*HEIGHT );
   for ( I32  y = 0;  y < I32(HEIGHT);  ++y )
      for ( I32  x = 0;  x < I32(WIDTH);  ++x )
         uniform_noise[noise_index(x,y)] = { noise(rng)+(x*CELL_SIZE), noise(rng)+(y*CELL_SIZE) };
   return uniform_noise;
}

Void Voronoi::generate_diagram( RNG &rng, std::function<F32(V2f const&, V2f const&)> const &distance_function ) noexcept {
   V2f   curr_pos;
   Size  best_match;
   F32   best_distance;
   // computes whether a cell is a better match than the current best
   // mutates best_match and best_distance if a better match is found.
   auto process = [&]( Size const &cell_index ) {
      V2f const &cell_center = noise[cell_index];
      F32        distance    = distance_function( cell_center, curr_pos );
      if ( distance < best_distance ) {
         best_distance = distance;
         best_match    = cell_index;
      }
   };

// main loop
   Size const  DIAGRAM_WIDTH  = Size(CELL_SIZE) * WIDTH,
               DIAGRAM_HEIGHT = Size(CELL_SIZE) * HEIGHT;
   for ( curr_pos.x=0;  curr_pos.x<DIAGRAM_WIDTH;  ++curr_pos.x ) {
      for ( curr_pos.y=0;  curr_pos.y<DIAGRAM_HEIGHT;  ++curr_pos.y ) {
         U16  cell_x = U16(curr_pos.x/CELL_SIZE),
              cell_y = U16(curr_pos.y/CELL_SIZE);
         // start with center cell (current cell):
         best_match    = noise_index(cell_x,cell_y);
         best_distance = distance_function( noise[best_match], curr_pos );
         // check if any of the neighbouring cells are better matches (if eligible):
         Bool const  w_border = (cell_x == 0),
                     e_border = (cell_x == WIDTH  - 1),
                     n_border = (cell_y == 0),
                     s_border = (cell_y == HEIGHT - 1);
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
               process( noise_index(cell_x+1,cell_y+1) );
         }
         if ( !w_border )    // W
            process( noise_index(cell_x-1,cell_y) );
         if ( !e_border )    // E
            process( noise_index(cell_x+1,cell_y) );
         diagram[diagram_index(U16(curr_pos.x), U16(curr_pos.y))] = best_match;
      }
   }
}

// TODO: refactor out?
Size Voronoi::compute_cell_real_estate_area( U16 const cell_id, Map const &map ) const noexcept {
   assert( cell_id < (WIDTH * HEIGHT) && "Cell ID is too low!" );

   Size  counter { 0 };

   Bounds  cell; {  /* first we calculate the relevant 1x1~3x3 cell matrix: */
      V2u  center_cell { cell_id % WIDTH, cell_id / WIDTH };
      cell.min.x = center_cell.x > 0        ?  center_cell.x-1  :  center_cell.x;
      cell.max.x = center_cell.x < WIDTH-1  ?  center_cell.x+1  :  center_cell.x;
      cell.min.y = center_cell.y > 0        ?  center_cell.y-1  :  center_cell.y;
      cell.max.y = center_cell.y < HEIGHT-1 ?  center_cell.y+1  :  center_cell.y;
   };


   for ( U16  y = cell.min.y * CELL_SIZE, y_end = (cell.max.y+1) * CELL_SIZE;  y < y_end;  ++y )
      for ( U16  x = cell.min.x * CELL_SIZE, x_end = (cell.max.x+1) * CELL_SIZE;  x < x_end;  ++x )
            if ( (diagram[diagram_index(x,y)] == cell_id)
            and (map.getTileAt(x,y) == Tile::ground) )
               ++counter;

   return counter;
}

// TODO: refactor out?
Voronoi::Bounds Voronoi::compute_cell_bounds( U16 const cell_id ) const noexcept {
   assert( cell_id < (WIDTH * HEIGHT) && "Cell ID is too low!" );

   Bounds result {};

   Bounds cell; {  /* first we calculate the relevant 1x1~3x3 cell matrix: */
      V2u center_cell { cell_id % WIDTH, cell_id / WIDTH };
      cell.min.x = center_cell.x > 0        ?  center_cell.x-1  : center_cell.x;
      cell.max.x = center_cell.x < WIDTH-1  ?  center_cell.x+1  : center_cell.x;
      cell.min.y = center_cell.y > 0        ?  center_cell.y-1  : center_cell.y;
      cell.max.y = center_cell.y < HEIGHT-1 ?  center_cell.y+1  : center_cell.y;
   };

//min_x_search: // find min x by probing column by column from the left
   for ( U16 x = cell.min.x * CELL_SIZE, x_end = (cell.max.x+1) * CELL_SIZE;  x < x_end;  ++x ) {
      for ( U16 y = cell.min.y * CELL_SIZE, y_end = (cell.max.y+1) * CELL_SIZE;  y < y_end;  ++y ) {
         if ( diagram[diagram_index(x,y)] == cell_id ) {
            result.min.x = x;
            goto max_x_search;
         }   
      }
   }

max_x_search: // find max x by probing column by column from the right
   for ( U16 x = (cell.max.x+1) * CELL_SIZE - 1, x_end = cell.min.x * CELL_SIZE;  x > x_end;  --x ) {
      for ( U16 y = cell.min.y * CELL_SIZE, y_end = (cell.max.y+1) * CELL_SIZE;  y < y_end;  ++y ) {
         if ( diagram[diagram_index(x,y)] == cell_id ) {
            result.max.x = x;
            goto min_y_search;;
         }   
      }
   }

min_y_search: // find min y by probing row by row from the top
   for ( U16 y = cell.min.y * CELL_SIZE, y_end = (cell.max.y+1) * CELL_SIZE;  y < y_end;  ++y ) {
      for ( U16 x = cell.min.x * CELL_SIZE, x_end = (cell.max.x+1) * CELL_SIZE;  x < x_end;  ++x ) {
         if ( diagram[diagram_index(x,y)] == cell_id ) {
            result.min.y = y;
            goto max_y_search;;
         }   
      }
   }

max_y_search: // find max y by probing row by row from the bottom
   for ( U16 y = (cell.max.y+1) * CELL_SIZE - 1, y_end = cell.min.y * CELL_SIZE;  y > y_end;  --y ) {
      for ( U16 x = cell.min.x * CELL_SIZE, x_end = (cell.max.x+1) * CELL_SIZE;  x < x_end;  ++x ) {
         if ( diagram[diagram_index(x,y)] == cell_id ) {
            result.max.y = y;
            goto end;
         }   
      }
   }

end:
   return result;
}