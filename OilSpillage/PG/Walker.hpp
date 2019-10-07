#pragma once

#include <functional>
#include <cassert>
#include "defs.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Direction.hpp"

struct WalkerGenArgs; // forward declaration
using SchedulerCallback = std::function<void(WalkerGenArgs&&)>; // alias

// -1 seed => random seed
struct WalkerGenArgs {
      U16                x,
                         y;
      Dir                dir;
      U8                 cur_depth,
                         max_depth;
      U16                min_len,
                         max_len;
      F32                child_len_factor,
                         turn_prob,
                         child_turn_prob_factor,
                         branch_prob,
                         child_branch_prob_factor;
      I32                seed;
      Map               &map;
      RNG               &rng;
      SchedulerCallback  scheduler_cb;
};

WalkerGenArgs create_child_args( WalkerGenArgs &parent_args,
                                 U16            x,
                                 U16            y,
                                 Dir            d );



class Branch {
public:
   Branch( WalkerGenArgs  args )
   :
      args(args),
      x(args.x),
      y(args.y),
      dir(args.dir)
   {
      U16_Dist  gen_tiles_to_walk( args.min_len, args.max_len );
      tiles_to_walk = gen_tiles_to_walk( args.rng );
   }

   Branch( Branch &&other ):
      args( std::move(other.args) ),
      is_done_generating( other.is_done_generating ),
      x( other.x ),
      y( other.y ),
      dir( other.dir )
   {}

   Branch( Branch const &other ) = delete;


   inline Bool  is_done() const { return is_done_generating; };

   Void walk( U16 steps );

private:
// data member variables:
   WalkerGenArgs  args;
   // status flag:
   Bool           is_done_generating = false;
   // RNG:
   F32_Dist       gen_selection { .0f, 100.0f };
   // target:
   U16            tiles_to_walk,
                  tiles_walked            = 0,
                  tiles_since_last_branch = 0,
                  tiles_since_last_turn   = 0,
                  x,
                  y;
   Dir            dir;
};



// usage:
//    create a Map
//    create a Walker with the generation parameters and map as arguments
//    call Walker::generate()
//    discard walker
class Walker {
public:
   // sets the necessary data members,
   // transfers the generation parameters 'args' to the root branch,
   // notifies it of the callback function to use (method  'schedule_branch')
   // then hands it a reference to the map it's to work on.
   Walker( Map &map,
           U8   max_depth,
           U16  min_len,
           U16  max_len,
           F32  len_fac,
           F32  turn_prob,
           F32  turn_prob_fac,
           F32  branch_prob,
           F32  branch_prob_fac,
           I32  seed );


   // generates the tree, one depth at a time, one tile per branch at a time
   Void generate();

private:
   // TODO: increasing depth value starting at 0!
   // schedules a branch at a given depth to be generated
   void schedule_branch( WalkerGenArgs && );
   void clean_isles() noexcept;
   void clean_ladders_x() noexcept;

   U8                max_depth;
   Map              &map;
   RD                rd;
   RNG               rng;
   Vec<Vec<Branch>>  branch_tree;
};
