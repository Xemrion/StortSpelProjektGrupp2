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
using RoadGenSchedulerCallback = std::function<void(RoadGenBranchArgs&&)>; // alias

// -1 seed => random seed
struct RoadGenBranchArgs {
      U16                       startX,
                                startY;
      Direction                 startDirection;
      U8                        currentDepth;
      U16                       currentLengthMin,
                                currentLengthMax;
      F32                       currentTurnProbability,
                                currentBranchProbability;
      Map                      &map;
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
   inline Bool    isDone() const { return isDoneGenerating; };
   Void           walk( MapConfig const & );

   RoadGenBranchArgs const args;

private:
   Bool           isDoneGenerating     { false };
   F32_Dist       generateSelection    { .0f, 1.0f };
   U16            tilesToWalk,
                  tilesWalked          { 0 },
                  tilesSinceLastBranch { 0 },
                  tilesSinceLastTurn   { 0 },
                  currentX,
                  currentY;
   Direction      currentDirection;
};



// usage:
//    create a Map
//    create a Walker with the generation parameters and map as arguments
//    call RoadGenerator::generate()
//    discard walker
class RoadGenerator {
public:
   // sets the necessary data members,
   // transfers the generation parameters 'args' to the root branch,
   // notifies it of the callback function to use (method  'scheduleBranch')
   // then hands it a reference to the map it's to work on.
   RoadGenerator( Map & );
   // generates the tree, one depth at a time, one tile per branch at a time
   Void generate( MapConfig const & );
   V2u  getStartPosition() const noexcept;

private:
   // TODO: increasing depth value starting at 0!
   // schedules a branch at a given depth to be generated
   void scheduleBranch( RoadGenBranchArgs && );
   void cleanIsles() noexcept;

   Map              &map;
   RD                rd;
   RNG               rng;
   Vec<Vec<Branch>>  branchTree;
};
