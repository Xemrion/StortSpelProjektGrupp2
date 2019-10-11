#include <cmath>
#include <cassert>
#include "utils.hpp"
#include "Voronoi.hpp"

Voronoi::Voronoi( RNG &rng, U8 cellSize, U16 width, U16 height, EuclideanDistanceTag ):
   cellSize ( cellSize ),
   width    ( width    ),
   height   ( height   ),
   diagram  ( Size(width) * height * cPow<Size>(cellSize, 2) ),
   noise    ( generateNoise(rng) )
{  // distance function (NOTE! the result is squared but this isn't an issue!)
   auto euclideanDistance {[]( V2f const &a, V2f const &b ) -> F32 {
	                          return std::sqrt(F32( std::pow(std::abs(a.x - b.x),2) + std::pow(std::abs(a.y - b.y),2) ));
                          }};
   // generate Voronoi diagram with Manhattan distance
   generateDiagram( rng, euclideanDistance );
}

Voronoi::Voronoi( RNG &rng, U8 cellSize, U16 width, U16 height, ManhattanDistanceTag ):
   cellSize ( cellSize ),
   width    ( width    ),
   height   ( height   ),
   diagram  ( Size(width) * height * cPow<Size>(cellSize, 2) ),
   noise    ( generateNoise(rng) )
{  // distance function
   auto manhattanDistance {[]( V2f const &a, V2f const &b ) -> F32 {
	                          return F32( std::abs(a.x - b.x) + std::abs(a.y - b.y) );
                          }};
   // generate Voronoi diagram with Manhattan distance
   generateDiagram( rng, manhattanDistance );
}

Vec<V2f> Voronoi::generateNoise( RNG &rng ) const noexcept {
   F32_Dist  noise( .0f, cellSize );
   Vec<V2f>  uniform_noise( Size(width)*height );
   for ( I32  y = 0;  y < I32(height);  ++y )
      for ( I32  x = 0;  x < I32(width);  ++x )
         uniform_noise[noiseIndex( x, y )] = { noise(rng)+(x*cellSize), noise(rng)+(y*cellSize) };
   return uniform_noise;
}

Void  Voronoi::generateDiagram( RNG &rng, std::function<F32(V2f const&, V2f const&)> const &distanceFunction ) noexcept {
   V2f   currentPosition;
   Size  bestMatch;
   F32   nearestDistance;
   // computes whether a cell is a better match than the current best
   // mutates bestMatch and nearestDistance if a better match is found.
   auto process  {[&]( Size const &cellIndex ) {
                     V2f const &cell_center  { noise[cellIndex] };
                     F32        distance     { distanceFunction( cell_center, currentPosition ) };
                     if ( distance < nearestDistance ) {
                        nearestDistance = distance;
                        bestMatch       = cellIndex;
                     }
                 }};

   // main loop
   Size const  diagramWidth   { Size(cellSize) * width  },
               diagramHeight  { Size(cellSize) * height };
   for ( currentPosition.x = 0;  currentPosition.x < diagramWidth;  ++currentPosition.x ) {
      for ( currentPosition.y = 0;  currentPosition.y < diagramHeight;  ++currentPosition.y ) {
         U16  cellX  { U16(currentPosition.x / cellSize) },
              cellY  { U16(currentPosition.y / cellSize) };
         // start with center cell (current cell):
         bestMatch       = noiseIndex( cellX, cellY );
         nearestDistance = distanceFunction( noise[bestMatch], currentPosition );
         // check if any of the neighbouring cells are better matches (if eligible):
         Bool const  isBorderingWest   { cellX ==          0 },
                     isBorderingEast   { cellX == width  - 1 },
                     isBorderingNorth  { cellY == 0          },
                     isBorderingSouth  { cellY == height - 1 };
         if ( not isBorderingNorth ) {
            process( noiseIndex( cellX, cellY-1 ) );
            if ( not isBorderingWest )
               process( noiseIndex( cellX-1, cellY-1 ) );
            if ( not isBorderingEast )
               process( noiseIndex( cellX+1, cellY-1 ) );
         }
         if ( not isBorderingSouth ) {
            process( noiseIndex( cellX, cellY+1 ) );
            if ( not isBorderingWest )
               process( noiseIndex( cellX-1, cellY+1 ) );
            if ( not isBorderingEast )
               process( noiseIndex( cellX+1, cellY+1 ) );
         }
         if ( not isBorderingWest )
            process( noiseIndex( cellX-1, cellY ) );
         if ( not isBorderingEast )
            process( noiseIndex( cellX+1, cellY ) );
         diagram[diagramIndex( U16(currentPosition.x), U16(currentPosition.y))] = bestMatch;
      }
   }
}

// TODO: refactor out?
Size  Voronoi::computeCellRealEstateArea( U16 const cellId, TileMap const &map ) const noexcept {
   assert( cellId < (width * height) && "Cell ID is too low!" );

   Size    counter { 0 };

   Bounds  cell; {  /* first we calculate the relevant 1x1~3x3 cell matrix: */
      V2u  centerCell { cellId % width, cellId / width };
      cell.min.x = centerCell.x > 0        ?  centerCell.x-1  :  centerCell.x;
      cell.max.x = centerCell.x < width-1  ?  centerCell.x+1  :  centerCell.x;
      cell.min.y = centerCell.y > 0        ?  centerCell.y-1  :  centerCell.y;
      cell.max.y = centerCell.y < height-1 ?  centerCell.y+1  :  centerCell.y;
   };

   for ( U16  y = cell.min.y * cellSize, yEnd = (cell.max.y+1) * cellSize;  y < yEnd;  ++y )
      for ( U16  x = cell.min.x * cellSize, xEnd = (cell.max.x+1) * cellSize;  x < xEnd;  ++x )
            if ( (diagram[diagramIndex( x, y )] == cellId)
            and (map.tileAt( x, y ) == Tile::ground) )
               ++counter;

   return counter;
}

// TODO: refactor out?
Voronoi::Bounds  Voronoi::computeCellBounds( U16 const cellId ) const noexcept {
   assert( cellId < (width * height) && "Cell ID is too low!" );

   Bounds result {};

   Bounds cell; {  /* first we calculate the relevant 1x1~3x3 cell matrix: */
      V2u centerCell { cellId % width, cellId / width };
      cell.min.x = centerCell.x > 0        ?  centerCell.x-1  : centerCell.x;
      cell.max.x = centerCell.x < width-1  ?  centerCell.x+1  : centerCell.x;
      cell.min.y = centerCell.y > 0        ?  centerCell.y-1  : centerCell.y;
      cell.max.y = centerCell.y < height-1 ?  centerCell.y+1  : centerCell.y;
   };

//min_x_search: // find min x by probing column by column from the left
   for ( U16 x = cell.min.x * cellSize, xEnd = (cell.max.x+1) * cellSize;  x < xEnd;  ++x ) {
      for ( U16 y = cell.min.y * cellSize, yEnd = (cell.max.y+1) * cellSize;  y < yEnd;  ++y ) {
         if ( diagram[diagramIndex( x, y )] == cellId ) {
            result.min.x = x;
            goto max_x_search;
         }   
      }
   }

max_x_search: // find max x by probing column by column from the right
   for ( U16 x = (cell.max.x+1) * cellSize - 1, xEnd = cell.min.x * cellSize;  x > xEnd;  --x ) {
      for ( U16 y = cell.min.y * cellSize, yEnd = (cell.max.y+1) * cellSize;  y < yEnd;  ++y ) {
         if ( diagram[diagramIndex( x, y )] == cellId ) {
            result.max.x = x;
            goto min_y_search;
         }   
      }
   }

min_y_search: // find min y by probing row by row from the top
   for ( U16 y = cell.min.y * cellSize, yEnd = (cell.max.y+1) * cellSize;  y < yEnd;  ++y ) {
      for ( U16 x = cell.min.x * cellSize, xEnd = (cell.max.x+1) * cellSize;  x < xEnd;  ++x ) {
         if ( diagram[diagramIndex( x, y )] == cellId ) {
            result.min.y = y;
            goto max_y_search;;
         }   
      }
   }

max_y_search: // find max y by probing row by row from the bottom
   for ( U16 y = (cell.max.y+1) * cellSize - 1, yEnd = cell.min.y * cellSize;  y > yEnd;  --y ) {
      for ( U16 x = cell.min.x * cellSize, xEnd = (cell.max.x+1) * cellSize;  x < xEnd;  ++x ) {
         if ( diagram[diagramIndex( x, y )] == cellId ) {
            result.max.y = y;
            goto end;
         }   
      }
   }

end:
   return result;
}
