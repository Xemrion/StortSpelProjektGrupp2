#include "RoadGenerator.hpp"

Branch::Branch( RoadGenBranchArgs args ):
   args              ( args                ),
   currentX          ( args.startX         ),
   currentY          ( args.startY         ),
   currentDirection  ( args.startDirection )
{
	U16 minLength = args.map.config.roadLengthFactorMin * args.map.config.dimensions.x * args.map.config.dimensions.y,
	    maxLength = args.map.config.roadLengthFactorMax * args.map.config.dimensions.x * args.map.config.dimensions.y;
	U16_Dist  gen_tiles_to_walk( minLength, maxLength );
	tilesToWalk = gen_tiles_to_walk( args.rng );
}


Branch::Branch( Branch &&other ) noexcept:
   args             ( std::move(other.args)  ),
   isDoneGenerating ( other.isDoneGenerating ),
   currentX         ( other.currentX         ),
   currentY         ( other.currentY         ),
   currentDirection ( other.currentDirection ),
   tilesToWalk      ( other.tilesToWalk      )
{}


RoadGenBranchArgs  createChildArgs( RoadGenBranchArgs const &parentArgs,
                                    U16                      startX,
                                    U16                      startY,
                                    Direction                startDirection )
{
   return RoadGenBranchArgs {
       startX,
       startY,
       startDirection,
(U8)  (parentArgs.currentDepth + 1),
(F32) (parentArgs.map.config.roadLengthFactor            * parentArgs.currentLengthFactorMin),
(F32) (parentArgs.map.config.roadLengthFactor            * parentArgs.currentLengthFactorMax),
       parentArgs.map.config.roadTurnProbabilityFactor   * parentArgs.currentTurnProbability,
       parentArgs.map.config.roadBranchProbabilityFactor * parentArgs.currentBranchProbability,
       parentArgs.map,
       parentArgs.rng,
       parentArgs.schedulerCallback
   };
}

void  Branch::walk( MapConfig const &config ) {
   auto  steps = config.roadStepSize;
// main loop:
   while ( (tilesWalked != tilesToWalk) and (steps --> 0) ) {
      // potential branch:
      Bool  isBranchEligible = ( args.currentDepth < args.map.config.roadDepthMax )
                                 and ( tilesSinceLastBranch >= (U16)config.roadMinTilesBeforeBranch );
      if ( isBranchEligible and generateSelection(args.rng) > args.map.config.roadBranchProbability ) {
         Direction childBranchDirection = generateSelection(args.rng) < .5f ?
                                          turnLeft(currentDirection) : turnRight(currentDirection);
         args.schedulerCallback( createChildArgs(args, currentX, currentY, childBranchDirection ) );
         tilesSinceLastBranch = 0;
      }
      else ++tilesSinceLastBranch;

      // potential turn:
      Bool  is_turn_eligible = tilesSinceLastTurn >= static_cast<U16>(config.roadMinTilesBeforeTurn);
      if ( is_turn_eligible and generateSelection(args.rng) > args.currentTurnProbability ) {
         currentDirection = generateSelection(args.rng) < .5f ?
                     turnLeft(currentDirection) : turnRight(currentDirection);
         tilesSinceLastTurn = 0;
      } else ++tilesSinceLastTurn;

      // walk!
      if ( !args.map.walk(currentX, currentY, currentDirection, Tile::road) ) {
         // if we can't walk forward, try turning a random direction first
         Bool  turned_left;
         if ( generateSelection(args.rng) < .5f ) {
            turned_left      = true;
            currentDirection = turnLeft( currentDirection );
         } else {
            turned_left      = false;
            currentDirection = turnRight( currentDirection );
         }
         if ( !args.map.walk(currentX, currentY, currentDirection, Tile::road) ) {
            // if we can't walk forward in that direction, try the other
            currentDirection = turn_around(currentDirection);
            if ( !args.map.walk(currentX, currentY, currentDirection, Tile::road) ) {
               currentDirection = turned_left ? turnRight(currentDirection) : turnLeft(currentDirection);
               if ( !args.map.walk(currentX, currentY, currentDirection, Tile::road) ) {
                  // TODO: join with logging system
                  #ifdef _DEBUG
                     std::cerr << "[RoadGen] WARNING! Branch had nowhere to go\n";
                  #endif
                  isDoneGenerating = true;
                  return;
               }
            }
         }
      }
      ++tilesWalked;
   }
   if ( tilesWalked == tilesToWalk )
      isDoneGenerating = true;
}



void  RoadGenerator::scheduleBranch( RoadGenBranchArgs &&args ) {
   assert( args.currentDepth < args.map.config.roadDepthMax && "Depth out of bounds!" ); 
   branchTree[args.currentDepth].emplace_back(std::move(args));
}

RoadGenerator::RoadGenerator( TileMap &map ):
   map ( map  ),
   rng ( rd() )
{

   branchTree.reserve( map.config.roadDepthMax );
   for ( U8 depth = 0;  depth < map.config.roadDepthMax;  ++depth ) {
		branchTree.push_back({});
         branchTree[depth].reserve( util::pow<U32>(8U,depth) ); // OPTI
	}
   // set up distributions
   U16_Dist  generateX         (   0, static_cast<U16>(map.width  / 2) );
   U16_Dist  generateY         (   0, static_cast<U16>(map.height / 2) );
   F32_Dist  generateSelection ( .0f, 1.0f );
   
   // seed == -1  =>  use random seed (non-procedural)
   if ( map.config.seed != -1 )
      rng.seed( map.config.seed ); 
   
   U16        startX, startY;
   Direction  startDirection;
   F32        selection = generateSelection( rng );
   // first we select which axis' side we'll start on (<50 means y, >=50 means x )
   if ( selection < .5f ) {
      startX         = generateX( rng );
      startY         = selection < .25f ?  0 : static_cast<U16>(map.height-1); // select y side to start on
      startDirection = selection < .25f ?  Direction::south : Direction::north;
   } else {
      startY         = generateY( rng );
      startX         = selection < .75f ?  0 : static_cast<U16>(map.width-1); // select x side to start on
      startDirection = selection < .75f ?  Direction::east : Direction::west;
   }
   using namespace std::placeholders;
   auto schedulerCallback = std::bind( &RoadGenerator::scheduleBranch, this, _1 );
   // setup args container
   RoadGenBranchArgs args { startX,
                            startY,
                            startDirection,
                            0, // startDepth
                      (F32) map.config.roadLengthFactorMin,
                      (F32) map.config.roadLengthFactorMax,
                            map.config.roadTurnProbability,
                            map.config.roadBranchProbability,
                            map,
                            rng,
                            schedulerCallback };
   // schedule main branch (and indirectly all child branches)
   scheduleBranch( std::move(args) );
}


// generates the tree, one depth at a time, one tile per branch at a time
void  RoadGenerator::generate( MapConfig const &config ) {
#ifdef _DEBUG_W_TERM
   U16 _DEBUG_iteration = 0;
#endif
   for ( U8 depth = 0U;  depth < map.config.roadDepthMax-1; ++depth ) {
      Bool  allDone = false;
      while ( !allDone ) {
         allDone = true;
         for ( auto  &branch : branchTree[depth] ) {
            if ( !branch.isDone() ) {
               branch.walk( config );
            }
            allDone &= branch.isDone();
         }
         #ifdef _DEBUG_W_TERM
            std::system("clear");
            std::cout << map                                 << "\n"
                      << "      Depth: " << depth            << "\n"
                      << "       Step: " << _DEBUG_iteration << "\n"
                      << "  Step size: " << STEP_SIZE        << "\n";
            // sleep hack 
            for ( I32 i = 0;  i < 500'000;  ++i ) {
               i = i+1;
               std::cerr << (i < 0 ? " ":"");
            }
         #endif
      }
   }
	start.x = branchTree[0][0].args.startX;
	start.y = branchTree[0][0].args.startY;
   cleanIsles();
}

// clean-up roads:
void  RoadGenerator::cleanIsles() noexcept {
   for ( auto y = 1U;  y < map.width-1;  ++y )
      for ( auto x = 1U;  x < map.height-1;  ++x )
         if ( map.tileAt( x,   y-1 ) == Tile::road
          and map.tileAt( x+1, y-1 ) == Tile::road
          and map.tileAt( x+1, y   ) == Tile::road
          and map.tileAt( x+1, y+1 ) == Tile::road
          and map.tileAt( x,   y+1 ) == Tile::road
          and map.tileAt( x-1, y+1 ) == Tile::road
          and map.tileAt( x-1, y   ) == Tile::road
          and map.tileAt( x-1, y-1 ) == Tile::road )
            { map.tileAt( x, y ) = Tile::ground; }
}

V2u  RoadGenerator::getStartPosition() const noexcept {
	return start;
}

void RoadGenerator::upscale() noexcept
{
	// TODO: slumpa deadends
	MapConfig  newConfig   { map.config };
	newConfig.dimensions = { map.config.dimensions.x*2, map.config.dimensions.y*2 };
	TileMap    newMap      { newConfig };
	for ( auto y = 0;  y < map.height;  ++y ) 
		for ( auto x = 0;  x < map.width;  ++x ) {
			newMap.data[newMap.index(x*2,y*2)] = map.data[map.index(x,y)];
			if ( x*2+1 < newMap.width )
				newMap.data[newMap.index(x*2+1,y*2)]
					= (map.data[map.index(x,y)] == Tile::road
						and (x+1 < map.width and map.data[map.index(x+1,y)]==Tile::road))
					? Tile::road : Tile::ground;
			if ( y*2+1 < newMap.height )
				newMap.data[newMap.index(x*2,y*2+1)]
					= (map.data[map.index(x,y)] == Tile::road
						and (y+1 < map.height and map.data[map.index(x,y+1)]==Tile::road))
					? Tile::road : Tile::ground;
	}
	start.x *= 2;
	start.y *= 2;
	map = std::move(newMap);
}
