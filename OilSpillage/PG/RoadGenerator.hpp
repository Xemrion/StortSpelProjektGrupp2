#pragma once

#include <functional>
#include <cassert>
#include "defs.hpp"
#include "MapConfig.hpp"
#include "utils.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Direction.hpp"

struct RoadGenBranchArgs; // forward declaration
using  RoadGenSchedulerCallback = std::function<void(RoadGenBranchArgs&&)>; // alias

// -1 seed => random seed
struct RoadGenBranchArgs {
      U16                       startX,
                                startY;
      Direction                 startDirection;
      U8                        currentDepth;
      F32                       currentLengthFactorMin,
                                currentLengthFactorMax,
                                currentTurnProbability,
                                currentBranchProbability;
      TileMap                  &map;
      RNG                      &rng;
      RoadGenSchedulerCallback  schedulerCallback;
};

RoadGenBranchArgs createChildArgs( RoadGenBranchArgs const &parentArgs,
                                   U16                      startX,
                                   U16                      startY,
                                   Direction                startDirection );

class Branch {
public:
                Branch( RoadGenBranchArgs );
                Branch( Branch && ) noexcept;
                Branch( Branch const & ) = delete;
   inline Bool  isDone() const noexcept { return isDoneGenerating; };
   void         walk( MapConfig const & );

   RoadGenBranchArgs const args;

private:
   Bool       isDoneGenerating     { false };
   F32_Dist   generateSelection    { .0f, 1.0f };
   U16        tilesToWalk,
              tilesWalked          { 0 },
              tilesSinceLastBranch { 0 },
              tilesSinceLastTurn   { 0 },
              currentX,
              currentY;
   Direction  currentDirection;
};



// usage:
//    create a TileMap
//    create a Walker with the generation parameters and map as arguments
//    call RoadGenerator::generate()
//    discard walker
class RoadGenerator {
public:
   // sets the necessary data members,
   // transfers the generation parameters 'args' to the root branch,
   // notifies it of the callback function to use (method  'scheduleBranch')
   // then hands it a reference to the map it's to work on.
         RoadGenerator( TileMap & );
   // generates the tree, one depth at a time, one tile per branch at a time
   void  generate( MapConfig const & );
   V2u   getStartPosition() const noexcept;

private:
   // TODO: increasing depth value starting at 0!
   // schedules a branch at a given depth to be generated
   void  scheduleBranch( RoadGenBranchArgs && );
   void  cleanIsles() noexcept;

   TileMap          &map;
   RD                rd;
   RNG               rng;
   Vector<Vector<Branch>>  branchTree;
};
