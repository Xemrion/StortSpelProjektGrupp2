#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include <functional>
#include <cassert>
#include "defs.hpp"
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
      I32                seed; // unused? TODO: I32?
      Map               &map;
      RNG               &rng;
      SchedulerCallback  scheduler_cb;
};

WalkerGenArgs create_child_args( WalkerGenArgs &parent_args,
                                 U16            x,
                                 U16            y,
                                 Dir            d )
{
   return WalkerGenArgs {
       x,
       y,
       d,
(U8)  (parent_args.cur_depth + 1),
       parent_args.max_depth,
(U16) (parent_args.child_len_factor * parent_args.min_len),
(U16) (parent_args.child_len_factor * parent_args.max_len),
       parent_args.child_len_factor,
       parent_args.turn_prob * parent_args.child_turn_prob_factor,
       parent_args.child_turn_prob_factor,
       parent_args.branch_prob * parent_args.child_branch_prob_factor,
       parent_args.child_branch_prob_factor,
       parent_args.seed,
       parent_args.map,
       parent_args.rng,
       parent_args.scheduler_cb
   };
}



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


   Bool  is_done() const { return is_done_generating; };

   Void walk( U16 steps ) {
   // main loop:
      while ( (tiles_walked != tiles_to_walk) and (steps-->0) ) {
         // potential branch:
         Bool  is_branch_eligible = ( args.cur_depth < args.max_depth-1 )
            and ( tiles_since_last_branch >= MIN_TILES_BEFORE_BRANCH );
            ;
         if ( is_branch_eligible and gen_selection(args.rng) > args.branch_prob ) {
            // TODO: delay by putting params in a vector and doing the branches after the main loop
            Dir branch_direction = gen_selection(args.rng) < 50 ?
                                   turn_left(dir) : turn_right(dir); // TODO: what if turn is invalid?
            args.scheduler_cb( create_child_args(args, x, y, branch_direction ) );
            tiles_since_last_branch = 0;
         }
         else ++tiles_since_last_branch;

         // potential turn:
         Bool  is_turn_eligible = tiles_since_last_turn > 1; // TODO: magic5
         if ( is_turn_eligible and gen_selection(args.rng) > args.turn_prob ) {
            dir = gen_selection(args.rng) < 50 ?
                        turn_left(dir) : turn_right(dir);
            tiles_since_last_turn = 0;
         } else ++tiles_since_last_turn;

         // walk!
         if ( !args.map.walk(x, y, dir, road_tile_at_depth(args.cur_depth)) ) {
            // if we can't walk forward, try turning a random direction first
            Bool  turned_left;
            if ( gen_selection(args.rng) < 50 ) {
               turned_left = true;
               dir = turn_left( dir );
            } else {
               turned_left = false;
               dir = turn_right( dir );
            }
            if ( !args.map.walk(x, y, dir, road_tile_at_depth(args.cur_depth)) ) {
               // if we can't walk forward in that direction, try the other
               dir = turn_around(dir);
               if ( !args.map.walk(x, y, dir, road_tile_at_depth(args.cur_depth)) ) {
                  dir = turned_left ? turn_right(dir) : turn_left(dir);
                  if ( !args.map.walk(x, y, dir, road_tile_at_depth(args.cur_depth)) ) {
                     // TODO: join with logging system
#ifdef _DEBUG
                     std::cerr << "[RoadGen] WARNING! Branch had nowhere to go\n";
#endif
                     is_done_generating = true;
                     return;
                  }
               }
            }
         }
         ++tiles_walked;
      }
      if ( tiles_walked == tiles_to_walk ) {
         is_done_generating = true;
      }
   }

private:
// constants:
   static U16 constexpr MIN_TILES_BEFORE_BRANCH = 4; // TODO: extract
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
           I32  seed )
   :
      max_depth ( max_depth ),
      map       ( map       ),
      rng       ( rd()      )
   {

      branch_tree.reserve( max_depth );
      for ( U8 depth = 0;  depth < max_depth;  ++depth ) {
		  branch_tree.push_back({});
          branch_tree[depth].reserve( cPow<U32>(8U,depth) ); // OPTI
	  }
      // set up distributions
      U16_Dist  gen_x         ( 0,  map.width  / 2 );
      U16_Dist  gen_y         ( 0,  map.height / 2 );
      F32_Dist  gen_selection ( 0,             100 );
      // set seed if not -1
      if ( seed != -1 )
         rng.seed( seed ); 
      // generate start position
      U16  x, y;
      Dir  dir;
      F32  selection = gen_selection(rng);
      // first we select which axis' side we'll start on (<50 => y, >=50 => x )
      if ( selection < 50 ) {
         x = gen_x(rng);
         if ( selection < 25 ) {
            y   = 0;
            dir = Dir::south; 
         }
         else {
            y   = map.height - 1;
            dir = Dir::north;
         }
         y = selection < 25 ? 0 : map.height - 1; // select y side to start on
      } else {
         if ( selection < 75 ) {
            x   = 0;
            dir = Dir::east;
         }
         else {
            x   = map.width - 1;
            dir = Dir::west;
         }
         y = gen_y(rng);
      }
      using namespace std::placeholders;
      auto scheduler_cb = std::bind( &Walker::schedule_branch, this, _1 );
      // setup args container
      WalkerGenArgs args { x,
                           y,
                           dir,
                           0,
                           max_depth,
                           min_len,
                           max_len,
                           len_fac,
                           turn_prob,
                           turn_prob_fac,
                           branch_prob,
                           branch_prob_fac,
                           seed,
                           map,
                           rng,
                           scheduler_cb };
      // schedule main branch (and indirectly all child branches)
      schedule_branch( std::move(args) );
   }


   // TODO: magic1
   // generates the tree, one depth at a time, one tile per branch at a time
   Void generate() {
#ifdef _DEBUG_W_TERM
      U16 _DEBUG_iteration = 0;
#endif
      for ( U8 depth = 0U;  depth < max_depth; ++depth ) {
         Bool  all_done = false;
         while ( !all_done) {
            all_done = true;
            for ( auto  &branch : branch_tree[depth] ) {
               if ( !branch.is_done() ) {
                  branch.walk( STEP_SIZE );
               }
               all_done &= branch.is_done();
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
   }

   // TODO: increasing depth value starting at 0!
   // schedules a branch at a given depth to be generated
   void schedule_branch( WalkerGenArgs &&args ) {
      assert( args.cur_depth < args.max_depth && "Depth out of bounds!" ); 
      branch_tree[args.cur_depth].emplace_back(std::move(args)); // TODO: verify
   }

private:
   static U16 constexpr STEP_SIZE = 1; // TODO: magic1
   U8                max_depth;
   Map const        &map;
   RD                rd;
   RNG               rng;
   Vec<Vec<Branch>>  branch_tree;
};
