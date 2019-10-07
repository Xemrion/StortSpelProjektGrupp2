#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "PG/config.hpp"
#include "PG/Voronoi.hpp"
#include "PG/Map.hpp"
#include "PG/Walker.hpp"

Graphics Game::graphics = Graphics();

#ifdef _DEBUG
std::ofstream building_logs ("PG/logs/building_generation.txt");
#endif

void Game::generateMap( I32 const seed ) {
	// create blank map
   map = std::make_unique<Map>( config::map_width, config::map_height );

	// debug output
#define NO_TERMINAL_COLORS // TODO: remove?
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
	         //   building_logs << "\t\t"   "Generating building #" << district_building_count++ << ". (Map total building count: " << total_building_count++ << ")\n"
            //                 << "\t\t\t" "Building size: "       << building_size << " tiles. (Map total tile count: " << (total_building_tile_count+=building_size) << ")\n"
            //                 << "\t\t\t" "The district real estate coverage is now " << (compute_curr_coverage() * 100.0f) << "%\n"
            //                 << "\t\t\t" "The target coverage is: " << target_coverage * 100.0f << "%\n";
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
            else {
               break; // TODO?
            }
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


void Game::initiateAStar() {
	// aStar.setMap(*map); // TODO
}

Game::Game()
{

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
	Sound::Init();
	UserInterface::init();
	this->menu.initUI();

	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow( window->handle );

	graphics.loadShape(SHAPE_CUBE);
	//graphics.loadTexture("brickwall.tga");
	//graphics.loadModel("Dummy_Roller_Melee");

	aiObject = std::make_unique<AIPlayer>();
	aiObject->mesh = graphics.getMeshPointer("Cube");
	aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	//aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	graphics.addToDraw(aiObject.get());
	//AiTestObject = new GameObject;
	//AiTestObject->mesh = graphics.getMeshPointer("Cube");
	//AiTestObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	//AiTestObject->setColor(Vector4(1.0f, 0.0f,0.0f,1.0f));
	//graphics.addToDraw(AiTestObject);

	parentTest = std::make_unique<GameObject>();
	parentTest->mesh = graphics.getMeshPointer("Cube");
	parentTest->setPosition(Vector3(-3.0f, 0.0f, 0.0f));
	parentTest->setScale(Vector3(0.5f, 0.5f, 0.5f));
	parentTest->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	graphics.addToDraw(parentTest.get());

	childTest = std::make_unique<GameObject>();
	childTest->mesh = graphics.getMeshPointer("Cube");
	childTest->setPosition(Vector3(-3.0f, 0.0f, 0.0f));
	childTest->setColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	childTest->parent = parentTest.get();
	graphics.addToDraw(childTest.get());

	// TODO refactor out (TODO: loadModel, make it just take the folder name)
	graphics.loadModel("Roads/Road_pavement");
	graphics.loadModel("Roads/Road_deadend");
	graphics.loadModel("Roads/Road_bend");
	graphics.loadModel("Roads/Road_straight");
	graphics.loadModel("Roads/Road_3way");
	graphics.loadModel("Roads/Road_4way");
	generateMap( config::START_SEED );

#ifdef _DEBUG
	// light tests
	lightList.addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 50.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 50.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 50.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 50.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	lightList.removeLight(lightList.addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));
	
	for (int i = 0; i < 50; ++i)
	{
		Vector3 randPos   = Vector3( float(rand() % 101 - 50), .01f, float(rand() % 101 - 50));
		Vector3 randColor = Vector3( float(rand()), float(rand()), float(rand() / RAND_MAX) );
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		lightList.addLight(
			PointLight(
				randPos,
				randColor,
				15.0f));
	}
#endif
	lightList.setSun(Sun(Vector3(0.0f, -1.0f, 1.0f), Vector3(1.0f, 0.8f, 0.6f)));
	graphics.setLightList(&lightList);

	player.init();

	playerLight = lightList.addLight(SpotLight(player.getVehicle()->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 50.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	RadioButtonValue = 0;
}


void Game::run()
{
	// Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)& countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	// Initial previous time	
	prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)& prevTime);

	std::vector<CinematicPos> points = {
		{ Vector3(0.0f, 30.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), 0.0f },
		{ Vector3(0.0f, 15.0f, 0.0f), Vector3(XM_PIDIV2, 0.0f, 0.0f), 3.0f }
	};
	this->camera.startCinematic(&points, false);

	

	this->aiObject->setTargetPos(this->player.getVehicle()->getPosition());
	Input::SetKeyboardPlayerID(0);

	while (this->window->update())
	{
		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::Update();
		Sound::Update(deltaTime);

		auto mouse = this->mouse->GetState();
		if (Input::CheckButton(Keys::CONFIRM,PRESSED,0))
		{
			Sound::PlaySoundEffect(L"test.wav");
		}
		if (Input::IsKeyDown_DEBUG(Keyboard::E))
			deltaTime /= 4;
		if (Input::CheckButton(Keys::L_TRIGGER, PRESSED, 0))
		{
			Sound::PlaySoundEffect(L"test2.wav");
			Input::SetRumble(0,0.5f,0.0f);
		}
		if (Input::CheckButton(Keys::L_TRIGGER, RELEASED, 0))
		{
			Sound::PlaySoundEffect(L"test2.wav");
			Input::ResetRumble(0);
		}

		Vector3 spotlightDir = Vector3((sin(player.getVehicle()->getRotation().y)), 0, (cos(player.getVehicle()->getRotation().y)));
		Vector3 spotlightPos = Vector3(player.getVehicle()->getPosition().x, player.getVehicle()->getPosition().y + 1, player.getVehicle()->getPosition().z);
		spotlightPos += spotlightDir;
		playerLight->setDirection(spotlightDir);
		playerLight->setPos(spotlightPos);

		this->aiObject->Update(deltaTime);
		this->camera.update(deltaTime);
		this->camera.setPosition(this->player.getVehicle()->getPosition() + Vector3(0, 25, 0));
		
		this->parentTest->setRotation(Vector3(0.0f, cos(curTime * secPerCount) * 2.0f, 0.0f));

		this->graphics.render(&this->camera);
 		//this->graphics.getdebugger()->DrawCube(this->testObject2->getTheAABB().maxPos, this->testObject2->getTheAABB().minPos, this->testObject2->getPosition(), Vector3(0, 1, 0));
		//this->menu.update(deltaTime);
		
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//imgui button, slider etc
		ImGui::Begin("OilSpillage");
		ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Driving Mode:");
		ImGui::RadioButton("Directional Semi-Realistic", &RadioButtonValue, 0);
		ImGui::RadioButton("Realistic", &RadioButtonValue, 1);
		if (RadioButtonValue == 0 && player.getDrivingMode() == 1) {
			player.setDrivingMode(0);
		}
		if (RadioButtonValue == 1 && player.getDrivingMode() == 0) {
			player.setDrivingMode(1);
		}
		Vector3 camPos = camera.getPosition();
		Vector3 camRot = camera.getRotation();
		Vector2 lDir = Input::GetDirectionL(0);
		Vector2 rDir = Input::GetDirectionR(0);
		float lStr = Input::GetStrengthL(0);
		float rStr = Input::GetStrengthR(0);
		bool status[4] = { Input::CheckButton(CONFIRM, UP, 0), Input::CheckButton(CONFIRM, HELD, 0), Input::CheckButton(CONFIRM, RELEASED, 0), Input::CheckButton(CONFIRM, PRESSED, 0) };
		ImGui::Text(("Cam Pos: " + std::to_string(camPos.x) + " " + std::to_string(camPos.y) + " " + std::to_string(camPos.z)).c_str());
		ImGui::Text(("Cam Rot: " + std::to_string(camRot.x) + " " + std::to_string(camRot.y) + " " + std::to_string(camRot.z)).c_str());
		ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text(("\n-- PLAYER 0 --\nConfirm Status - Up: " + std::to_string(status[0]) + " Held: " + std::to_string(status[1]) + " Released: " + std::to_string(status[2]) + " Pressed: " + std::to_string(status[3])).c_str());
		ImGui::Text(("L Dir: " + std::to_string(lDir.x) + " " + std::to_string(lDir.y)).c_str());
		ImGui::Text(("L Str: " + std::to_string(lStr)).c_str());
		ImGui::Text(("R Dir: " + std::to_string(rDir.x) + " " + std::to_string(rDir.y)).c_str());
		ImGui::Text(("R Str: " + std::to_string(rStr)).c_str());
		ImGui::Text(("Accelerator: " + std::to_string(player.getAcceleratorX())).c_str());

		//static int rotation[3] = { 0, 0, 0 };
		//ImGui::SliderInt3("Cam Rotation", rotation, -360, 360);
		//this->camera.setRotation(Vector3(rotation[0] * (DirectX::XM_PI / 180), rotation[1] * (DirectX::XM_PI / 180), rotation[2] * (DirectX::XM_PI / 180)));

		ImGui::End();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		this->graphics.presentScene();
		
		player.update(deltaTime);

		/*Vector3 tempPos = AiTestObject->getPosition();
		Vector4 tempColor = AiTestObject->getColor();
		this->AI.update(player.getVehicle()->getPosition(), deltaTime, tempPos, tempColor);
		AiTestObject->setPosition(tempPos);
		AiTestObject->setColor(tempColor);*/
		this->aiObject->Update(deltaTime);
		//deltaTime reset
		prevTime = curTime;
	}
	// TODO: RAII
	delete this->testNetwork;
}
