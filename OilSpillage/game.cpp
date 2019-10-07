#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "States/MenuGameState.h"
#include "States/PlayingGameState.h"
#include "PG/Walker.hpp"

Graphics Game::graphics = Graphics();

std::unique_ptr<GameState> Game::states[STATECOUNT];

int Game::currentState = STATE_MENU;
int Game::oldState     = -1;



GameState* Game::getCurrentState()
{
	return states[currentState].get();
}

void Game::setState(State state)
{
	int newState = static_cast<int>(state);
	if (oldState == -1 && currentState != newState)
	{
		oldState = currentState;
		currentState = newState;
   }
}



#ifdef _DEBUG
std::ofstream building_logs ("PG/logs/building_generation.txt");
#endif
void Game::generateMap( I32 const seed ) {
	// create blank map
   map = std::make_unique<Map>( config::map_width, config::map_height );

   // debug output
#define NO_TERMINAL_COLORS
#ifdef _DEBUG
	std::ofstream f1("PG/logs/road_gen_debug_output_pregen.txt");
	if (f1.is_open()) {
		f1 << *map;
		f1.close();
	}
#endif

	Walker roadnet {
	  *map,      // the map to work on (mutate)
		4,        // depth; number of generations     (max 4 for now)
		80,       // min length of a branch           (number of tiles)
	   120,      // max length of a branch           (number of tiles)
		0.5f,     // child length factor              (e.g. 0.5 => the length will halve per generation)
		4.0f,     // turn probability                 (e.g. 0.75 = 0.75%)
		2.f,      // child turn probability factor    (multiplicative)
		12.0f,    // branch probability               (e.g. 0.75 = 0.75%)
		0.75f,    // child branch probability factor  (multiplicative)
		seed      // seed
	};

	roadnet.generate();

	// debug output
#ifdef _DEBUG
	std::ofstream f2("PG/logs/road_gen_debug_output.txt");
	if (f2.is_open()) {
		f2 << *map;
		f2.close();
	}
}
   // TODO: remove when no longer needed for debugging
   Vec<Vector4>  rgba_tbl {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.5f, 1.0f },
      { 0.0f, 0.0f, 1.0f, 1.0f },

      { 0.0f, 0.5f, 0.0f, 1.0f },
      { 0.0f, 0.5f, 0.5f, 1.0f },
      { 0.0f, 0.5f, 1.0f, 1.0f },

      { 0.0f, 1.0f, 0.0f, 1.0f },
      { 0.0f, 1.0f, 0.5f, 1.0f },
      { 0.0f, 1.0f, 1.0f, 1.0f },

      { 0.5f, 0.0f, 0.0f, 1.0f },
      { 0.5f, 0.0f, 0.5f, 1.0f },
      { 0.5f, 0.0f, 1.0f, 1.0f },

      { 0.5f, 0.5f, 0.0f, 1.0f },
      { 0.5f, 0.5f, 0.5f, 1.0f },
      { 0.5f, 0.5f, 1.0f, 1.0f },

      { 0.5f, 1.0f, 0.0f, 1.0f },
      { 0.5f, 1.0f, 0.5f, 1.0f },
      { 0.5f, 1.0f, 1.0f, 1.0f },

      { 1.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 0.0f, 0.5f, 1.0f },
      { 1.0f, 0.0f, 1.0f, 1.0f },

      { 1.0f, 0.5f, 0.0f, 1.0f },
      { 1.0f, 0.5f, 0.5f, 1.0f },
      { 1.0f, 0.5f, 1.0f, 1.0f },

      { 1.0f, 1.0f, 0.0f, 1.0f },
      { 1.0f, 1.0f, 0.5f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f }
   };
      for ( auto &rgba : rgba_tbl )
         rgba = blend( rgba, Vector4{1.0f}, 0.5f );
#endif

	// game object instantiation
	road_tiles = map->load_as_models(graphics); // TODO: RAII!

   RD  rd;
   RNG rng( rd() );
   rng.seed(seed);
   voronoi = std::make_unique<Voronoi>( rng,
                                        config::CELL_SIZE,
                                        map->width  / config::CELL_SIZE,
                                        map->height / config::CELL_SIZE,
                                        Voronoi::ManhattanDistanceTag{} );

#ifdef _DEBUG
   // display noise centers:
   markers.reserve( voronoi->noise.size() );
   for ( auto const &cell_center : voronoi->noise ) {
      markers.emplace_back();
      auto &cell_center_marker = markers.back();
      cell_center_marker.mesh  = graphics.getMeshPointer("Cube");
      cell_center_marker.setColor({1, 0, 0, 1});
      cell_center_marker.setScale({.2f, 4, .2f});
      cell_center_marker.setPosition({ map->tile_xy_to_world_pos(U16(cell_center.x), U16(cell_center.y))});
      graphics.addToDraw(&cell_center_marker);
   }
#endif


   for ( U16 y=0;  y < map->height;  ++y ) {
      for ( U16 x=0;  x < map->width;  ++x ) {
         auto &tile = road_tiles[map->index(x,y)];
		   graphics.addToDraw(&tile);
#ifdef _DEBUG
         // colour code tiles depending on cell index:
         assert( map->data.size() == voronoi->diagram.size() && "BUG!" );
         auto        cell_idx = voronoi->diagram_index(x,y);
         auto        cell_id  = voronoi->diagram[cell_idx];
         auto const &color    = rgba_tbl[ cell_id % rgba_tbl.size()];
         tile.setColor( color );
#endif
		   tile.setScale( Vector3{ 0.0005f * config::TILE_SIDE_LENGTH,
                                 0.0001f * config::TILE_SIDE_LENGTH,
                                 0.0005f * config::TILE_SIDE_LENGTH }); // TODO: scale models instead
      }
	}
   generateBuildings(rng);
#ifdef _DEBUG
   if ( building_logs.is_open() )
      building_logs.close();
#endif
}

void Game::generateBuildings( RNG &rng) {

#ifdef _DEBUG
   assert( building_logs.is_open() );
   U32  total_building_count      = 0;
	building_logs << "=========================================== BEGINNING BUILDING GENERATION ===========================================\n";
#endif
   U32  total_building_tile_count = 0; // TODO refactor into _DEBUG bloack


   U16_Dist gen_offset    (   0, 256  );
   F32_Dist gen_selection ( .0f, 1.0f );
   // TODO: embed into MapGenerator to ensure by design that voronoi and map are generated first

   auto cell_id_to_district_enum = [&gen_offset,&rng]( U16 const ID ) {
      return static_cast<District>( (ID+gen_offset(rng)) % static_cast<U16>(District::size) );
   };

   // generate look-up table (TODO: refactor)
   Vec<District> district_tbl { voronoi->noise.size(), District::park };
   for ( auto i = 0;  i < district_tbl.size();  ++i )
      district_tbl[i] = cell_id_to_district_enum(i);

   for ( U16  cell_id = 0;  cell_id < voronoi->noise.size();  ++cell_id ) {
      District    district_type         { district_tbl[cell_id] };
      Size        curr_area             { 0 };
      Size const  cell_area             { voronoi->compute_cell_real_estate_area(cell_id,*map) };

#ifdef _DEBUG
      U64 district_building_count = 0;
	   building_logs << "\n\t" "Generating buildings for district #" << cell_id << " (TYPE: \"" << stringify(district_type) << "\")\n";
#endif

      if ( (cell_area != 0) and (district_type != District::park) ) {
         auto        compute_curr_coverage = [&curr_area, cell_area]() { return F32(curr_area)/F32(cell_area); };
         F32  const  target_coverage       { get_building_density(district_type) };
         while ( compute_curr_coverage() < target_coverage ) {
            auto potential_lot = find_valid_house_lot( rng, cell_id, *voronoi, *map, district_tbl );
            if ( potential_lot ) {
               auto &building      = potential_lot.value();
               auto  building_size = building.size();
#ifdef _DEBUG
	            building_logs << "\t\t"   "Generating building #" << district_building_count++ << ". (Map total building count: " << total_building_count++ << ")\n"
                             << "\t\t\t" "Building size: "       << building_size << " tiles. (Map total tile count: " << (total_building_tile_count+=building_size) << ")\n"
                             << "\t\t\t" "The district real estate coverage is now " << (compute_curr_coverage() * 100.0f) << "%\n"
                             << "\t\t\t" "The target coverage is: " << target_coverage * 100.0f << "%\n";
#endif
               U16_Dist floor_count ( get_min_height(district_type), get_max_height(district_type) );
               F32 rng_floor_count = F32( floor_count(rng) );
               curr_area += building_size;
               house_tiles.reserve( building_size );
               for ( auto &tile_coord : potential_lot.value() ) {
                  house_tiles.emplace_back();
                  auto &house_tile = house_tiles.back();
                  house_tile.mesh  = graphics.getMeshPointer("Cube");
                  house_tile.setColor( {.75f, .75f, .75f, 1.0f} );
                  house_tile.setScale( { 0.5f * config::TILE_SIDE_LENGTH,
                                         0.5f * config::TILE_HEIGHT * rng_floor_count,
                                         0.5f * config::TILE_SIDE_LENGTH } );
                  house_tile.setPosition( { map->tile_xy_to_world_pos(U16(tile_coord.x), U16(tile_coord.y)) } );
                  ++total_building_tile_count;
               }
            }
            else break; // TODO?
         }
      }
   }

   // adding all the tiles to draw:
   for ( auto &e : house_tiles )
      graphics.addToDraw(&e);
}

// basic proto placement algorithm
// TODO: refactor out of Game
Opt<Vec<V2u>>  Game::find_valid_house_lot( RNG &rng, U16 cell_id, Voronoi const &district_map, Map &map, Vec<District> const &district_tbl ) {
   using Bounds = Voronoi::Bounds;
   Bounds    cell_bounds { district_map.compute_cell_bounds(cell_id) };
   U16_Dist  gen_offset  {               0x0,            0xFF };
   U16_Dist  gen_x       { cell_bounds.min.x, cell_bounds.max.x };
   U16_Dist  gen_y       { cell_bounds.min.y, cell_bounds.max.y };

   auto is_valid_position = [&]( V2u const &pos ) -> bool {
      return  (district_map.diagram[district_map.diagram_index(pos.x,pos.y)] != cell_id)
          or  (map.data[map.index(pos.x,pos.y)] != Tile::ground);
   };

   District const  district_type    { district_tbl[cell_id] }; // TODO: refactor? (and param)
   U16_Dist        gen_target_size  { get_building_min_area(district_type), get_building_max_area(district_type) };
   U16      const  target_size      { gen_target_size(rng) };

   // find valid starting coordinate:
   V2u  start_pos {};
   do {
      start_pos.x = gen_x(rng);
      start_pos.y = gen_y(rng);
   } while ( is_valid_position(start_pos) );

   Vec<V2u>  claimed_positions, candidate_srcs;

   claimed_positions.reserve( target_size );
   claimed_positions.push_back( start_pos );

   candidate_srcs.reserve( target_size );
   candidate_srcs.push_back( start_pos );

   while ( claimed_positions.size() != target_size  and  candidate_srcs.size() != 0 ) {
      auto const  &src_pos = candidate_srcs[ gen_offset(rng) % candidate_srcs.size() ];
      // find valid neighbours
      auto  candidate_dsts = map.get_neighbour_tile_coords(src_pos);
      // eliminate invalid ones from our destination candidate list
      candidate_dsts.erase( std::remove_if(candidate_dsts.begin(), candidate_dsts.end(), is_valid_position), candidate_dsts.end() );

      // if we found at least one valid destination position, pick one of the candiates
      if ( candidate_dsts.size() != 0 ) {
         // TODO: possibly improve results if weighted towards lower indices?
         auto  &match = candidate_dsts[ gen_offset(rng) % candidate_dsts.size() ];
         claimed_positions.push_back( match );
         candidate_srcs.push_back( match );
      } // otherwise remove the source position from the source candidates
      else candidate_srcs.erase( std::remove_if( candidate_srcs.begin(),
                                                 candidate_srcs.end(),
                                                 [&src_pos] (auto const &e) { return e==src_pos; }
                                 ),
                                 candidate_srcs.end()
      );
   }
   
   if ( claimed_positions.size() == target_size ) return claimed_positions;
   else return {};
}

Game::Game()
{
	states[STATE_MENU] = std::make_unique<MenuGameState>();
	states[STATE_PLAYING] = std::make_unique<PlayingGameState>();
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::init(Window* window)
{
	this->window = window; 
	graphics.init(window);
	Input::Init();
	Sound::Init();
	UserInterface::initStaticVariables();
}

void Game::run()
{
	// Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)& countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	//Initial previous time	
	this->prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)& this->prevTime);

	Input::SetKeyboardPlayerID(0);
	states[currentState]->init();

	while (this->window->update())
	{
		//deltaTime
		this->curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)& this->curTime);
		//Calculate deltaTime
		this->deltaTime = (this->curTime - this->prevTime) * this->secPerCount;

		Input::Update();
		Sound::Update(this->deltaTime);
		states[currentState]->update(deltaTime);

		if (oldState != -1)
		{
			states[oldState]->cleanUp();
			graphics.clearDraw();
			states[currentState]->init();

			oldState = -1;
		}

		//deltaTime reset
		this->prevTime = this->curTime;
	}
}
