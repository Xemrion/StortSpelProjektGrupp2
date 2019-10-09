#include "PlayingGameState.h"
#include "../Input.h"
#include "../Sound.h"

void PlayingGameState::generateMap( Config const &config ) {
	// TODO: this shit shouldn't be necessary. make it so GameObject's destructor handles this D:<
	for ( auto &e : roadTiles )
		graphics.removeFromDraw(&e);
	for ( auto &e : districtMarkers )
		graphics.removeFromDraw(&e);
	for ( auto &e : houseTiles )
		graphics.removeFromDraw(&e);

	roadTiles       = Vec<GameObject>( static_cast<size_t>(config.map_dimensions.x) * config.map_dimensions.y );
	districtMarkers = Vec<GameObject>( static_cast<size_t>(config.cell_side) * config.cell_side );
	houseTiles.clear();
	// create blank map
	map = std::make_unique<Map>( config );

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
	   *map,         // the map to work on (mutate)
		4,           // depth; number of generations     (max 4 for now)
		80,          // min length of a branch           (number of tiles)
		120,         // max length of a branch           (number of tiles)
		0.5f,        // child length factor              (e.g. 0.5 => the length will halve per generation)
		4.0f,        // turn probability                 (e.g. 0.75 = 0.75%)
		2.f,         // child turn probability factor    (multiplicative)
		12.0f,       // branch probability               (e.g. 0.75 = 0.75%)
		0.75f,       // child branch probability factor  (multiplicative)
		config.seed // seed
	};

	roadnet.generate(config);

	// debug output
#ifdef _DEBUG
	std::ofstream f2("PG/Logs/road_gen_debug_output_seed_" + std::to_string(config.seed) + ".txt");
	if (f2.is_open()) {
		f2 << *map;
		f2.close();
	}
#endif

	// game object instantiation
	roadTiles = map->load_as_models(graphics); // TODO: RAII!

	RD  rd;
	RNG rng( rd() );
	rng.seed(config.seed);
	districtMap = std::make_unique<Voronoi>( rng,
                                             config.cell_side,
                                             map->width  / config.cell_side,
                                             map->height / config.cell_side,
                                             Voronoi::ManhattanDistanceTag{} );

#ifdef _DEBUG
	// display noise centers:
	districtMarkers.reserve( districtMap->noise.size() );
	for ( auto const &cell_center : districtMap->noise ) {
		districtMarkers.emplace_back();
		auto &cell_center_marker = districtMarkers.back();
		cell_center_marker.mesh  = graphics.getMeshPointer("Cube");
		cell_center_marker.setColor({1, 0, 0, 1});
		cell_center_marker.setScale({.2f, 4, .2f});
		cell_center_marker.setPosition({ map->tile_xy_to_world_pos(U16(cell_center.x), U16(cell_center.y))});
		graphics.addToDraw(&cell_center_marker);
	}
#endif


	for ( U16 y=0;  y < map->height;  ++y ) {
		for ( U16 x=0;  x < map->width;  ++x ) {
			auto &tile = roadTiles[map->index(x,y)];
			graphics.addToDraw(&tile);
			tile.setScale( Vector3{ 0.0005f * config.tile_scale.x,
									0.0005f * config.tile_scale.y,
									0.0005f * config.tile_scale.z }); // TODO: scale models instead
		}
	}
	generateBuildings(config, rng);
}

void PlayingGameState::generateBuildings( Config const &config, RNG &rng) {

#ifdef _DEBUG
	std::ofstream building_logs ("PG/logs/building_generation.txt");
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
	Vec<District> district_tbl { districtMap->noise.size(), District::park };
	for ( auto i = 0;  i < district_tbl.size();  ++i )
		district_tbl[i] = cell_id_to_district_enum(i);

	for ( U16  cell_id = 0;  cell_id < districtMap->noise.size();  ++cell_id ) {
		District    district_type         { district_tbl[cell_id] };
		Size        curr_area             { 0 };
		Size const  cell_area             { districtMap->compute_cell_real_estate_area(cell_id,*map) };

#ifdef _DEBUG
		U64 district_building_count = 0;
		building_logs << "\n\t" "Generating buildings for district #" << cell_id << " (TYPE: \"" << stringify(district_type) << "\")\n";
#endif

		if ( (cell_area != 0) and (district_type != District::park) ) {
			auto        compute_curr_coverage = [&curr_area, cell_area]() { return F32(curr_area)/F32(cell_area); };
			F32  const  target_coverage       { get_building_density(district_type) };
			while ( compute_curr_coverage() < target_coverage ) {
			auto potential_lot = find_valid_house_lot( rng, cell_id, *districtMap, *map, district_tbl );
			if ( potential_lot ) {
				auto &building      = potential_lot.value();
				auto  building_size = building.size();
#ifdef _DEBUG
				building_logs << "\t\t"   "Generating building #" << district_building_count++ << ". (Map total building count: " << total_building_count++ << ")\n"
							  << "\t\t\t" "Building size: "       << building_size << " tiles. (Map total tile count: " << (total_building_tile_count+=U32(building_size)) << ")\n"
							  << "\t\t\t" "The district real estate coverage is now " << (compute_curr_coverage() * 100.0f) << "%\n"
							  << "\t\t\t" "The target coverage is: " << target_coverage * 100.0f << "%\n";
#endif
				U16_Dist floor_count ( get_min_height(district_type), get_max_height(district_type) );
				F32 rng_floor_count = F32( floor_count(rng) );
				curr_area += building_size;
				houseTiles.reserve( building_size );
				for ( auto &tile_coord : potential_lot.value() ) {
					houseTiles.emplace_back();
					auto &house_tile = houseTiles.back();
					house_tile.mesh  = graphics.getMeshPointer("Cube");
					house_tile.setColor( {.75f, .75f, .75f, 1.0f} );
					house_tile.setScale( { 0.5f * config.tile_scale.x,
										   0.5f * config.tile_scale.y * config.floor_height_factor * rng_floor_count,
										   0.5f * config.tile_scale.z } );
					house_tile.setPosition( { map->tile_xy_to_world_pos(U16(tile_coord.x), U16(tile_coord.y)) } );
					++total_building_tile_count;
				}
			}
			else break; // TODO?
			}
		}
	}

	// adding all the tiles to draw:
	for ( auto &e : houseTiles )
		graphics.addToDraw(&e);

#ifdef _DEBUG
	if ( building_logs.is_open() )
		building_logs.close();
#endif
}

// basic proto placement algorithm
// TODO: refactor out of Game
Opt<Vec<V2u>>  PlayingGameState::find_valid_house_lot( RNG &rng, U16 cell_id, Voronoi const &district_map, Map &map, Vec<District> const &district_tbl ) {
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

void PlayingGameState::toggleDistrictColors() noexcept {
	static auto toggled { false };
	if ( !toggled ) {
		Vec<Vector4> rgba_tbl {
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
			rgba = blend( rgba, Vector4{1.0f}, config.district_blend_factor );
		// colour code tiles depending on cell index:
		for ( U16 y=0;  y < map->height;  ++y ) {
			for ( U16 x=0;  x < map->width;  ++x ) {
			assert( map->data.size() == districtMap->diagram.size() && "BUG!" );
			auto       &tile     = roadTiles[map->index(x,y)];
			auto        cell_idx = districtMap->diagram_index(x,y);
			auto        cell_id  = districtMap->diagram[cell_idx];
			auto const &color    = rgba_tbl[ cell_id % rgba_tbl.size()];
			tile.setColor( color );
			}
		}
		for ( auto &e : houseTiles ) {
			auto        pos      = map->world_pos_to_tile_xy( e.getPosition() );
			auto        cell_idx = districtMap->diagram_index(pos.x,pos.y);
			auto        cell_id  = districtMap->diagram[cell_idx];
			auto const &color    = rgba_tbl[ cell_id % rgba_tbl.size()];
			e.setColor( color );
		}
	} else {
		static auto const default_color = Vector4 { .8f, .8f, .8f, 1.0f };
		for ( auto &e : roadTiles )
			e.setColor( default_color );
		for ( auto &e : houseTiles )
			e.setColor( default_color );
	}
	toggled = !toggled;
}

void PlayingGameState::initiateAStar()
{
	//this->aStar.setMap(*map); // TODO
}

PlayingGameState::PlayingGameState() : graphics(Game::getGraphics())
{
}

PlayingGameState::~PlayingGameState()
{
}

void PlayingGameState::init()
{
	lightList = std::make_unique<LightList>();
	player    = std::make_unique<Vehicle>();
	camera    = std::make_unique<DynamicCamera>();
	//testNetwork = std::make_unique<RoadNetwork>(2430, Vector2(16.0f, 16.0f), Vector2(-16.0f,-16.0f), 25); //Int seed, max pos, min pos, angle in degrees
	graphics.createFrustumBuffer(camera.get());

	aStar = std::make_unique<AStar>(20, 20, Vector2(-10, 10));
	graphics.loadMesh("Cube");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall");
	graphics.loadModel("Dummy_Roller_Melee");

	aiObject       = std::make_unique<Actor>();
	aiObject->mesh = graphics.getMeshPointer("Cube");
	aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	aiObject->setAStar(aStar.get());
	//aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	graphics.addToDraw(aiObject.get());

	graphics.loadModel("Roads/Road_pavement");
	graphics.loadModel("Roads/Road_deadend");
	graphics.loadModel("Roads/Road_bend");
	graphics.loadModel("Roads/Road_straight");
	graphics.loadModel("Roads/Road_3way");
	graphics.loadModel("Roads/Road_4way");
	generateMap(config);

#ifdef _DEBUG
	// light tests
	lightList->addLight(SpotLight(Vector3(-2.f,  1.0f,  0.0f), Vector3(1.0f, 1.0f, 1.0f), 1.f, Vector3(-2.f, -1.0f,  0.0f), 0.5));
	lightList->addLight(SpotLight(Vector3( 2.f,  1.0f,  0.0f), Vector3(0.3f, 0.3f, 1.0f), 1.f, Vector3( 2.f, -1.0f,  0.0f), 0.5));
	lightList->addLight(SpotLight(Vector3( 0.f,  1.0f,  2.0f), Vector3(1.0f, 0.3f, 0.3f), 1.f, Vector3( 0.f, -1.0f,  2.0f), 0.5));
	lightList->addLight(SpotLight(Vector3( 0.f,  1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 1.f, Vector3( 0.f, -1.0f, -2.0f), 0.5));
   
   /*
	//Road Network Turtlewalker
	testNetwork.get()->generateInitialSegments("FFFFFFFFFFFFFFF-FF-FF-FFH+F+F+FF+FF+FF+FFFFFFFFF+FF-F-FF-FFF-FFF");
	testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");
	testNetwork.get()->setAngle(45);
	for (int i = 0; i < 5; i++) {
		testNetwork.get()->generateAdditionalSegments("FFFF-FF+F+F+F", ((i * 3) + 1) + 2, false);
		testNetwork.get()->generateAdditionalSegments("H-F+FFF+F+H+F", ((i * i) + 1) + 2, true);
	}
	testNetwork.get()->cleanRoadNetwork();
	testNetwork.get()->saveTestNetwork("test-network");
   */

	lightList->removeLight(lightList->addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));

	for (int i = 0; i < 50; ++i) {
		Vector3 randPos   = Vector3(static_cast<float>(rand() % 101 - 50), static_cast<float>(rand() % 9 + 1), static_cast<float>(rand() % 101 - 50));
		Vector3 randColor = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand() / RAND_MAX));
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		lightList->addLight(
			PointLight(
				randPos,
				randColor,
				1.0f));
	}
#endif
	lightList->setSun(Sun(Vector3(0.0f, -1.0f, 1.0f), Vector3(1.0f, 0.8f, 0.6f)));

	graphics.setLightList(lightList.get());

	player->init();

	playerLight = lightList->addLight(SpotLight(player->getVehicle()->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	points = {
		{ Vector3(0.0f, 30.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), 0.0f },
		{ Vector3(0.0f, 15.0f, 0.0f), Vector3(XM_PIDIV2, 0.0f, 0.0f), 3.0f }
	};
	camera->startCinematic(&points, false);

	Input::SetKeyboardPlayerID(0);
}

void PlayingGameState::cleanUp()
{
	this->map.reset();
	this->aiObject.reset();
	this->districtMarkers.clear();
	this->roadTiles.clear();
	this->houseTiles.clear();
	this->lightList.reset();
	this->player.reset();
	this->districtMap.reset();
	this->camera.reset();
	this->points.clear();
	this->testNetwork.reset();
}

void PlayingGameState::ImGui_Driving() {
	ImGui::Begin("OilSpillage");
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Driving Mode:");
	static int radioButtonValue = 0;
	ImGui::RadioButton("Directional Semi-Realistic", &radioButtonValue, 0);
	ImGui::RadioButton("Realistic", &radioButtonValue, 1);
	//ImGui::RadioButton("Directional Smooth", &radioButtonValue, 2);
	ImGui::RadioButton("Old Directional Semi-Realistic", &radioButtonValue, 3);
	if (radioButtonValue == 0)
		player->setDrivingMode(0);
	else if (radioButtonValue == 1)
		player->setDrivingMode(1);
	/*else if (radioButtonValue == 2)
		player->setDrivingMode(2);*/
	else if (radioButtonValue == 3)
		player->setDrivingMode(3);

	Vector3 camPos = camera->getPosition();
	Vector3 camRot = camera->getRotation();
	Vector2 lDir   = Input::GetDirectionL(0);
	Vector2 rDir   = Input::GetDirectionR(0);
	float lStr     = Input::GetStrengthL(0);
	float rStr     = Input::GetStrengthR(0);
	bool status[4] = { Input::CheckButton(CONFIRM, UP, 0), Input::CheckButton(CONFIRM, HELD, 0), Input::CheckButton(CONFIRM, RELEASED, 0), Input::CheckButton(CONFIRM, PRESSED, 0) };
	ImGui::Text(("Cam Pos: " + std::to_string(camPos.x) + " " + std::to_string(camPos.y) + " " + std::to_string(camPos.z)).c_str());
	ImGui::Text(("Cam Rot: " + std::to_string(camRot.x) + " " + std::to_string(camRot.y) + " " + std::to_string(camRot.z)).c_str());
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(("\n-- PLAYER 0 --\nConfirm Status - Up: " + std::to_string(status[0]) + " Held: " + std::to_string(status[1]) + " Released: " + std::to_string(status[2]) + " Pressed: " + std::to_string(status[3])).c_str());
	ImGui::Text(("L Dir: " + std::to_string(lDir.x) + " " + std::to_string(lDir.y)).c_str());
	ImGui::Text(("L Str: " + std::to_string(lStr)).c_str());
	ImGui::Text(("R Dir: " + std::to_string(rDir.x) + " " + std::to_string(rDir.y)).c_str());
	ImGui::Text(("R Str: " + std::to_string(rStr)).c_str());
	ImGui::Text(("Accelerator: " + std::to_string(player->getAcceleratorX())).c_str());
   ImGui::End();
}

void PlayingGameState::ImGui_ProcGen() {
   ImGui::Begin("Map Generation:");
   ImGui::SetWindowSize({425,275});
   // debug colors toggle:
   static auto districtColorsToggled          = false,
               districtColorsToggledPrevFrame = false;
   ImGui::Checkbox("Toggle district colors", &districtColorsToggled );
   if ( districtColorsToggled != districtColorsToggledPrevFrame ) {
      toggleDistrictColors();
      districtColorsToggledPrevFrame = districtColorsToggled;
   }
   // map:
   ImGui::InputInt2(   "Map dimensions",      &config.map_dimensions.data[0]  );
   // (TODO! bugged!) ImGui::InputFloat3( "Tile sddcale",          &config.tile_scale.data[0]      );
   ImGui::InputFloat(  "Floor height factor", &config.floor_height_factor     );
   ImGui::InputInt(    "Seed",                &config.seed                    );
   ImGui::InputInt(    "District cell side",  &config.cell_side               );
   // roads:
   ImGui::InputInt(    "Tiles before turn",   &config.min_tiles_before_turn   );
   ImGui::InputInt(    "Tiles before branch", &config.min_tiles_before_branch );
   ImGui::InputInt(    "Road step size",      &config.road_step_size          );
            /// TODO: add walker_gen_args params as well
   // regen button:
   if ( ImGui::Button("Re-generate!") )
      generateMap(config);
   ImGui::End();
}

void PlayingGameState::update(float deltaTime)
{
   /*-------------------------UPDATING-------------------------*/
	if (Input::IsKeyDown_DEBUG(Keyboard::E)) {
		deltaTime /= 4;
	}
	player->update(deltaTime);

	Vector3 spotlightDir = Vector3((sin(player->getVehicle()->getRotation().y)), 0, (cos(player->getVehicle()->getRotation().y)));
	Vector3 spotlightPos = Vector3(player->getVehicle()->getPosition().x, player->getVehicle()->getPosition().y + 1, player->getVehicle()->getPosition().z);
	spotlightPos += spotlightDir * 1;
	playerLight->setDirection(spotlightDir);
	playerLight->setPos(spotlightPos);

	aiObject->update(deltaTime,player->getVehicle()->getPosition());
	camera->update(deltaTime);
	camera->setPosition(player->getVehicle()->getPosition() + Vector3(0, 25, 0));

	/*-------------------------RENDERING-------------------------*/
	//Render all objects
	graphics.render(camera.get());
	//testNetwork.get()->drawRoadNetwork(&graphics);

#ifdef _DEBUG
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui_Driving();
	ImGui_ProcGen();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
 #endif

	// Present scene
	graphics.presentScene();
}
