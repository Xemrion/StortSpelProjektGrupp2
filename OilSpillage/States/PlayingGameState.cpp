#include "PlayingGameState.h"
#include "../Input.h"
#include "../Sound.h"
#include "../PG/config.hpp"
#include "../PG/Voronoi.hpp"
#include "../PG/Map.hpp"
#include "../PG/Walker.hpp"

void PlayingGameState::generateMap() {
	// create blank map
	this->map = std::make_unique<Map>(64, 64);

	// debug output
#define NO_TERMINAL_COLORS // TODO: remove?
#ifdef _DEBUG
	std::ofstream f1("PG/Logs/road_gen_debug_output_pregen.txt");
	if (f1.is_open()) {
		f1 << *this->map;
		f1.close();
	}
#endif
	int aSeed = 607023;
	Walker generator{
	  *this->map,      // the map to work on (mutate)
		3,        // depth; number of generations     (max 4 for now)
		80,       // min length of a branch           (number of tiles)
	   120,      // max length of a branch           (number of tiles)
		0.9f,     // child length factor              (e.g. 0.5 => the length will halve per generation)
		0.5f,     // turn probability                 (e.g. 0.75 = 0.75%)
		1.0f,      // child turn probability factor    (multiplicative)
		5.0f,    // branch probability               (e.g. 0.75 = 0.75%)
		0.5f,    // child branch probability factor  (multiplicative)
		aSeed   // seed
	};

	generator.generate();

	// debug output
#ifdef _DEBUG
	std::ofstream f2("PG/Logs/road_gen_debug_output_seed_" + std::to_string(aSeed) + ".txt");
	if (f2.is_open()) {
		f2 << *this->map;
		f2.close();
	}
	// TODO: remove when no longer needed for debugging
	Vec<Vector4>  rgba_tbl{
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
	for (auto& rgba : rgba_tbl)
		rgba = blend(rgba, Vector4{ 1.0f }, 0.5f);
#endif

	// game object instantiation
	tiles = this->map->load_as_models(graphics); // TODO: RAII!

	RD  rd;
	RNG rng(rd());
	rng.seed(952895);
	auto voronoi = Voronoi(rng,
		config::CELL_SIZE,
		this->map->width / config::CELL_SIZE,
		this->map->height / config::CELL_SIZE,
		Voronoi::ManhattanDistanceTag{});

#ifdef _DEBUG
	// display noise centers:
	markers.reserve(voronoi.noise.size());
	for (auto const& cell_center : voronoi.noise) {
		markers.emplace_back();
		auto& cell_center_marker = markers.back();
		cell_center_marker.mesh = this->graphics.getMeshPointer("Cube");
		cell_center_marker.setColor({ 1, 0, 0, 1 });
		cell_center_marker.setScale({ .2f, 4, .2f });
		cell_center_marker.setPosition({ this->map->tile_xy_to_world_pos(U16(cell_center.x), U16(cell_center.y)) });
		this->graphics.addToDraw(&cell_center_marker);
	}
#endif


	for (U16 y = 0; y < this->map->height; ++y) {
		for (U16 x = 0; x < this->map->width; ++x) {
			auto& tile = tiles[this->map->index(x, y)];
			this->graphics.addToDraw(&tile);
#ifdef _DEBUG
			// colour code tiles depending on cell index:
			assert(this->map->data.size() == voronoi.diagram.size() && "BUG!");
			auto        cell_idx = voronoi.diagram_index(x, y);
			auto        cell_id = voronoi.diagram[cell_idx];
			auto const& color = rgba_tbl[cell_id % rgba_tbl.size()];
			tile.setColor(color);
#endif
			//tile.setTexture(graphics.getTexturePointer("brickwall.tga")); // TODO remove
			tile.setScale(Vector3{ 0.0005f * config::TILE_SIDE_LENGTH,
								  0.0001f * config::TILE_SIDE_LENGTH,
								  0.0005f * config::TILE_SIDE_LENGTH }); // TODO: scale models instead
		}
	}
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
	this->lightList = std::make_unique<LightList>();
	this->player = std::make_unique<Vehicle>();
	this->camera = std::make_unique<DynamicCamera>();
	this->testNetwork = std::make_unique<RoadNetwork>(2430, Vector2(16.0f, 16.0f), Vector2(-16.0f,-16.0f), 25); //Int seed, max pos, min pos, angle in degrees
	this->graphics.createFrustumBuffer(camera.get());

	this->graphics.loadMesh("sda");
	this->graphics.loadShape(SHAPE_CUBE);
	this->graphics.loadTexture("brickwall.tga");
	this->graphics.loadModel("Dummy_Roller_Melee");

	this->aiObject = std::make_unique<AIPlayer>();
	this->aiObject->mesh = this->graphics.getMeshPointer("Cube");
	this->aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	//aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	this->graphics.addToDraw(aiObject.get());


	// TODO reafctor out
	this->graphics.loadModel("Road_pavement");
	this->graphics.loadModel("Road_deadend");
	this->graphics.loadModel("Road_bend");
	this->graphics.loadModel("Road_straight");
	this->graphics.loadModel("Road_3way");
	this->graphics.loadModel("Road_4way");
	this->generateMap();

#if _DEBUG
	// light tests
	this->lightList->addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 1.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	this->lightList->addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 1.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	this->lightList->addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 1.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	this->lightList->addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	//Road Network Turtlewalker
	this->testNetwork.get()->generateInitialSegments("FFFFFFFFFFFFFFF-FF-FF-FFH+F+F+FF+FF+FF+FFFFFFFFF+FF-F-FF-FFF-FFF");
	this->testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");
	this->testNetwork.get()->setAngle(45);
	for (int i = 0; i < 5; i++) {
		this->testNetwork.get()->generateAdditionalSegments("FFFF-FF+F+F+F", ((i * 3) + 1) + 2, false);
		this->testNetwork.get()->generateAdditionalSegments("H-F+FFF+F+H+F", ((i * i) + 1) + 2, true);
	}
	this->testNetwork.get()->cleanRoadNetwork();
	this->testNetwork.get()->saveTestNetwork("test-network");

	this->lightList->removeLight(this->lightList->addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));

	for (int i = 0; i < 50; ++i)
	{
		Vector3 randPos = Vector3(static_cast<float>(rand() % 101 - 50), static_cast<float>(rand() % 9 + 1), static_cast<float>(rand() % 101 - 50));
		Vector3 randColor = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand() / RAND_MAX));
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		this->lightList->addLight(
			PointLight(
				randPos,
				randColor,
				1.0f));
	}
#endif
	this->lightList->setSun(Sun(Vector3(0.0f, -1.0f, 1.0f), Vector3(1.0f, 0.8f, 0.6f)));
	this->graphics.setLightList(this->lightList.get());

	this->player->init();

	this->playerLight = this->lightList->addLight(SpotLight(this->player->getVehicle()->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	this->points = {
		{ Vector3(0.0f, 30.0f, -10.0f), Vector3(0.0f, 0.0f, 0.0f), 0.0f },
		{ Vector3(0.0f, 15.0f, 0.0f), Vector3(XM_PIDIV2, 0.0f, 0.0f), 3.0f }
	};
	this->camera->startCinematic(&this->points, false);

	this->aiObject->setTargetPos(this->player->getVehicle()->getPosition());
	Input::SetKeyboardPlayerID(0);
}

template <typename T>
void delete_pointed_to(T* const ptr)
{
	delete ptr;
}

void PlayingGameState::cleanUp()
{
	this->map.reset();
	this->aiObject.reset();
	this->markers.clear();
	this->tiles.clear();
	this->lightList.reset();
	this->player.reset();
	this->camera.reset();
	this->points.clear();
	this->testNetwork.reset();
}

void PlayingGameState::update(float deltaTime)
{
	/*-------------------------UPDATING-------------------------*/
	
	if(Input::IsKeyDown_DEBUG(Keyboard::E)) {
		deltaTime /= 4;
	}
	this->player->update(deltaTime);
	Vector3 spotlightDir = Vector3((sin(this->player->getVehicle()->getRotation().y)), 0, (cos(this->player->getVehicle()->getRotation().y)));
	Vector3 spotlightPos = Vector3(this->player->getVehicle()->getPosition().x, this->player->getVehicle()->getPosition().y + 1, this->player->getVehicle()->getPosition().z);
	spotlightPos += spotlightDir * 1;
	this->playerLight->setDirection(spotlightDir);
	this->playerLight->setPos(spotlightPos);

	this->aiObject->Update(deltaTime);
	this->camera->update(deltaTime);
	this->camera->setPosition(this->player->getVehicle()->getPosition() + Vector3(0, 25, 0));

	/*-------------------------RENDERING-------------------------*/
	//Render all objects
	this->graphics.render(this->camera.get());
	this->testNetwork.get()->drawRoadNetwork(&this->graphics);

	//ImGui rendering --BEGIN--
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("OilSpillage");
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(("Vehicle velocity: "+ std::to_string(player->getVelocitySpeed())).c_str());

	ImGui::Text("Driving Mode:");

	static int radioButtonValue = 0;
	ImGui::RadioButton("Directional Semi-Realistic", &radioButtonValue, 0);
	ImGui::RadioButton("Realistic", &radioButtonValue, 1);
	ImGui::RadioButton("Directional Smooth", &radioButtonValue, 2);
	ImGui::RadioButton("Old Directional Semi-Realistic", &radioButtonValue, 3);
	if (radioButtonValue == 0) {
		player->setDrivingMode(0);
	}
	if (radioButtonValue == 1) {
		this->player->setDrivingMode(1);
	}
	if (radioButtonValue == 2) {
		this->player->setDrivingMode(2);
	}
	if (radioButtonValue == 3) {
		this->player->setDrivingMode(3);
	}
	Vector3 camPos = this->camera->getPosition();
	Vector3 camRot = this->camera->getRotation();
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
	ImGui::Text(("Accelerator: " + std::to_string(this->player->getAcceleratorX())).c_str());

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//ImGui rendering --END--

	//Present scene
	this->graphics.presentScene();
}
