#include "PlayingGameState.h"
#include "../Sound.h"
#include "../Input.h"
#include "../UI/Playing/UIPlaying.h"
#include "../UI/Playing/UIPaused.h"
#include "../UI/Menu/UIOptions.h"
#include "../PG/MinimapTextureGenerator.hpp"
#include "../PG/Profiler.hpp"
#include "../UI/Playing/UICompletedStage.h"
#include "../UI/Playing/UIBeforePlaying.h"
#include "../UI/Menu/UIControls.h"
#include "../profiling.h"

#define RELEASE_DEBUG

void PlayingGameState::fillTestParticle()
{
	this->colors[0] = this->graphics.getTestParticleSystem()->getColor(0).x;
	this->colors2[0] = this->graphics.getTestParticleSystem()->getColor(1).x;
	this->colors3[0] = this->graphics.getTestParticleSystem()->getColor(2).x;
	this->colors4[0] = this->graphics.getTestParticleSystem()->getColor(3).x;

	this->colors[1] = this->graphics.getTestParticleSystem()->getColor(0).y;
	this->colors2[1] = this->graphics.getTestParticleSystem()->getColor(1).y;
	this->colors3[1] = this->graphics.getTestParticleSystem()->getColor(2).y;
	this->colors4[1] = this->graphics.getTestParticleSystem()->getColor(3).y;

	this->colors[2] = this->graphics.getTestParticleSystem()->getColor(0).z;
	this->colors2[2] = this->graphics.getTestParticleSystem()->getColor(1).z;
	this->colors3[2] = this->graphics.getTestParticleSystem()->getColor(2).z;
	this->colors4[2] = this->graphics.getTestParticleSystem()->getColor(3).z;

	this->size1 = this->graphics.getTestParticleSystem()->getStartSize();
	this->size2 = this->graphics.getTestParticleSystem()->getEndSize();
	this->vectorFieldPower = this->graphics.getTestParticleSystem()->getVectorFieldPower();
	this->vectorFieldSize = this->graphics.getTestParticleSystem()->getVectorFieldSize();
}

void PlayingGameState::createElectric(int randNr, float deltaTime)
{


	electricNew = randNr;
	testNetwork->clearSegments();
	testNetwork.get()->generateInitialSegments("F");

	if (true)
		testNetwork.get()->setAngle(electricNew);


	testNetwork.get()->generateAdditionalSegments("F-F", 1, false);
	testNetwork.get()->setAngle(40);
	
	testNetwork.get()->generateAdditionalSegments("F-F+F-F", 2, false);
	testNetwork.get()->generateAdditionalSegments("F-F-F+F", 3, true);
}

void PlayingGameState::initAI()
{
	aStar = nullptr;
	aStar = std::make_unique<AStar>( map->getTileMap() );
	actorManager = nullptr;
	actorManager = std::make_unique<ActorManager>( aStar.get(), physics.get(), map.get(), &rng );
	aStar->generateTileData(map->getTileMap());
}

PlayingGameState::PlayingGameState(int seed,float time) : graphics(Game::getGraphics()), time(time), currentMenu(MENU_BEFORE_PLAYING)
{

	this->current_item = NULL;

	config.seed = seed;
	rng.seed(config.seed); // gör i konstruktorn
	lightList = std::make_unique<LightList>();
	camera = std::make_unique<DynamicCamera>();
	testNetwork = std::make_unique<RoadNetwork>(1351, Vector2(32.0f, 32.0f), Vector2(-32.0f,-32.0f), 0); //Int seed, max pos, min pos, angle in degrees
	graphics.createFrustumBuffer(camera.get());


	graphics.loadMesh("Cube");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadModel("Entities/Turret");
	graphics.loadModel("Entities/Player", Vector3(3.14f / 2, 0, 0));

	// clutter
	graphics.loadModel("Entities/Barrel");
	graphics.loadModel("Entities/Garbage_Bag");
	graphics.loadModel("Entities/Quad");
	graphics.loadTexture("brownPaperCardboard");

	//Skyscraper Textures
	graphics.loadTexture("Skyscrapers/wall01");
	graphics.loadTexture("Skyscrapers/window01");
	graphics.loadTexture("Skyscrapers/wall02");

	graphics.loadModel("Entities/Star");
	graphics.loadModel("Entities/Streetlight");

	graphics.loadModel("Tiles/Quad_SS"); // single-sided
	graphics.loadTexture("Tiles/asphalt");
	graphics.loadTexture("Tiles/asphalt_nor");
	graphics.loadTexture("Tiles/grasslands");
	graphics.loadTexture("Tiles/grasslands_nor");
	graphics.loadTexture("Tiles/ashlands");
	graphics.loadTexture("Tiles/ashlands_nor");
	graphics.loadTexture("Tiles/arctic");
	graphics.loadTexture("Tiles/arctic_nor");
	graphics.loadTexture("Tiles/desert");
	graphics.loadTexture("Tiles/desert_nor");
	graphics.loadTexture("Tiles/road_trans_2file2metro");
	graphics.loadTexture("Tiles/road_trans_2file2metro_nor");
	graphics.loadTexture("Tiles/sidewalk_corner_outer_ne");
	graphics.loadTexture("Tiles/sidewalk_corner_outer_ne_nor");
	graphics.loadTexture("Tiles/sidewalk_side_n");
	graphics.loadTexture("Tiles/sidewalk_side_n_nor");
	graphics.loadTexture("Tiles/sidewalk_corner_inner_ne");
	graphics.loadTexture("Tiles/sidewalk_corner_inner_ne_nor");
	graphics.loadTexture("Tiles/sidewalk_u_n");
	graphics.loadTexture("Tiles/sidewalk_u_n_nor");
	graphics.loadTexture("Tiles/sidewalk_hole");
	graphics.loadTexture("Tiles/sidewalk_hole_nor");
	graphics.loadTexture("Tiles/road_marker_straight_n");
	graphics.loadTexture("Tiles/road_marker_deadend_n");
	graphics.loadTexture("Tiles/road_marker_turn_ne");
	graphics.loadTexture("Tiles/road_marker_3way_n");
	graphics.loadTexture("Tiles/road_marker_4way");
	graphics.loadTexture("Tiles/concrete");
	graphics.loadTexture("Tiles/concrete_nor");
	graphics.loadTexture("Tiles/zebra_crossing");

	graphics.loadTexture("Tiles/road_2file_deadend_n");
	graphics.loadTexture("Tiles/road_2file_deadend_n_nor");
	graphics.loadTexture("Tiles/road_2file_turn_ne");
	graphics.loadTexture("Tiles/road_2file_turn_ne_nor");
	graphics.loadTexture("Tiles/road_2file_straight_n");
	graphics.loadTexture("Tiles/road_2file_straight_n_nor");
	graphics.loadTexture("Tiles/road_2file_3way_n");
	graphics.loadTexture("Tiles/road_2file_3way_n_nor");
	graphics.loadTexture("Tiles/road_2file_4way");
	graphics.loadTexture("Tiles/road_2file_4way_nor");
	// missing: single tile road!

	for ( auto &tileset: houseTilesets )
		for ( auto &part: std::vector{ "f_ic", "f_oc", "f_sa", "f_sb", "w_ic", "w_oc", "w_sa", "w_sb",
                                     "r_ic", "r_oc", "r_sa", "r_sb", "r_m" } )
			graphics.loadModel( "Houses/tilesets/" + tileset.name + "/" + part );

	loadMultitileLayouts("data/layouts/multitilePrefabs.dat");
	graphics.loadModel("Entities/Drone");
	
	std::string modelPath;
	modelPath = MODEL_ROOT_DIR;
	modelPath += "Entities/Roller_Melee";
	graphics.loadMesh(modelPath);
	graphics.loadMaterial("Entities/Roller_Melee");
	graphics.loadModel("Hospital");
	//graphics.loadModel("Entities/Roller_Melee");
	graphics.loadModel("Houses/testHouse");
	graphics.loadModel("Houses/testHouse2");
	graphics.loadModel("Houses/testHouse3");
	graphics.loadModel("Houses/testHouse4");
	graphics.loadModel("Houses/testHouse5");
	graphics.loadModel("Houses/testHouse6");
	graphics.loadMaterial("Houses/houseMaterial");
	graphics.loadMaterial("Houses/houseMaterial2");
	graphics.loadMaterial("Houses/houseMaterial3");
	graphics.loadModel("Entities/PowerUps/HealthBoost");
	graphics.loadModel("Entities/PowerUps/SpeedBoost");
	graphics.loadModel("Entities/PowerUps/TimeBoost");

	player = std::make_unique<Vehicle>();
	player->makePlayer();

	lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));

	graphics.setLightList(lightList.get());
	SpotLight tempLight(Vector3(0, 0, 0), Vector3(0.9, 0.5, 0), 1.0f, Vector3(0, 0, 0), 0.0f);
	this->player->setSpotLight(lightList->addLight(tempLight));

	physics = std::make_unique<Physics>();
	player->init(physics.get());

	map = std::make_unique<Map>(graphics, config, physics.get(), *lightList );
	// Minimap stuff
	auto tilemap = map->getTileMap();
	topLeft = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileSideScaleFactor / 2, .0f, config.tileSideScaleFactor / 2);
	bottomRight = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileSideScaleFactor / 2, 0, -config.tileSideScaleFactor / 2);
	// Needs to be loaded before the menues
	minimap = createMinimapTexture(*map);
	generateMapPowerUps();
	createFogOfWarTexture(*map);
	createDistanceTexture(*map);

	menues[MENU_PLAYING] = std::make_unique<UIPlaying>();
	menues[MENU_PLAYING]->init();
	menues[MENU_PAUSED] = std::make_unique<UIPaused>();
	menues[MENU_PAUSED]->init();
	menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	menues[MENU_COMPLETED_STAGE] = std::make_unique<UICompletedStage>();
	menues[MENU_BEFORE_PLAYING] = std::make_unique<UIBeforePlaying>();
	menues[MENU_CONTROLS] = std::make_unique<UIControls>();

	Vector3 startPos = map->getStartPositionInWorldSpace();
	player->setPosition(startPos + Vector3(.0f, 0.00f - 1.2f, .0f));
	player->getVehicleBody1()->setPosition(startPos + Vector3(.0f, 0.65f - 1.2f, .0f));

	this->cameraObject = new GameObject;
	Game::getGraphics().loadShape(Shapes::SHAPE_CUBE);
	cameraObject->mesh = Game::getGraphics().getMeshPointer("Cube");
	cameraObject->setPosition(Vector3(player->getPosition().x, 10.0f, player->getPosition().z));
	cameraObject->setScale(Vector3(1.5f, 1.5f, 1.5f));
	btRigidBody* tempo = physics->addSphere(cameraObject->getScale().x, btVector3(cameraObject->getPosition().x, cameraObject->getPosition().y, cameraObject->getPosition().z), 0.01f);
	//btRigidBody* tempo = physics->addBox(btVector3(cameraObject->getPosition().x, cameraObject->getPosition().y, cameraObject->getPosition().z), btVector3(0.5f, 0.5f, 0.5f), 10.0f);
	cameraObject->setRigidBody(tempo, physics.get());
	cameraObject->getRigidBody()->activate();
	cameraObject->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	cameraObject->getRigidBody()->setFriction(0);
	cameraObject->getRigidBody()->setGravity(btVector3(0, 0, 0));
	//cameraObject->getRigidBody()->setDamping(10,0);
	cameraTimer = 0;

	initAI();




	Game::getGraphics().setUISun(Vector3(1.0f, -1.0f, 0.1f), Vector4(0.84, 0.83, 0, 1));

	playerLight = lightList->addLight(SpotLight(player->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 2.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	points = {
		{
		 Vector3(0.0f, 30.0f + cameraDistance, -10.0f) + player->getPosition(),
		 Vector3(0.0f, 0.0f, 0.0f), 0.0f },
		{
		 Vector3(0.0f, cameraDistance, 0.0f) + player->getPosition(),
		 Vector3(XM_PIDIV2, 0.0f, 0.0f), 3.0f }
	};
	camera->startCinematic(&points, true);
	Vector4 colorP2[] = {
		Vector4(0.03f,0.03f,0.03f,1),
		Vector4(0.9f, 0.9f, 0.05f, 1)
	};

	graphics.setParticle2ColorNSize(colorP2, 2, 0.025f, 0.05f);

	this->graphics.setTestParticleSystem(this->graphics.getParticleSystem("explosion"));
	this->fillTestParticle();
	graphics.getParticleSystem("fire")->setGravity(-0.1f);
	graphics.getParticleSystem("fire")->setSize(0.055f, 0.065f);
	graphics.getParticleSystem("fire")->changeVectorField(1.75f, 0.5f);
	
	graphics.getParticleSystem("smoke")->setGravity(-0.1f);
	graphics.getParticleSystem("smoke")->setSize(0.055f, 0.065f);
	graphics.getParticleSystem("smoke")->changeVectorField(1.75f, 0.09f);

	if (map->getInfo().environment.getWeather() == Weather::snow) {
		Vector4 snowColor[4] = {
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(1.0f,1.0f,1.0f,1.0f)
		};
		graphics.getParticleSystem("snow")->setGravity(0.5f);
		graphics.getParticleSystem("snow")->changeVectorField(1.75f, 0.5f);
		graphics.getParticleSystem("snow")->changeColornSize(snowColor,4, 0.0f, 0.047f);
	}
	if (map->getInfo().environment.getWeather() == Weather::rain || map->getInfo().environment.getWeather() == Weather::thunderstorm) {
		Vector4 rainColor[4] = {
		Vector4(0.4f,0.6f,1.0f,1.0f),
		Vector4(0.4f,0.6f,1.0f,1.0f),
		Vector4(0.4f,0.6f,1.0f,1.0f),
		Vector4(0.4f,0.6f,1.0f,1.0f)
		};
		graphics.getParticleSystem("snow")->setGravity(10.0f);
		graphics.getParticleSystem("snow")->changeVectorField(1.75f, 0.5f);
		graphics.getParticleSystem("snow")->changeColornSize(rainColor, 4, 0.0f, 0.047f);
	}
	if (map->getInfo().environment.getWeather() == Weather::ashfall) {
		Vector4 ashColor[4] = {
		Vector4(0.9f,0.3f,0.0f,1.0f),
		Vector4(0.27f,0.27f,0.27f,1.0f),
		Vector4(0.27f,0.27f,0.27f,1.0f),
		Vector4(0.27f,0.27f,0.27f,1.0f)
		};
		graphics.getParticleSystem("snow")->setGravity(0.1f);
		graphics.getParticleSystem("snow")->changeVectorField(0.75f, 3.0f);
		graphics.getParticleSystem("snow")->changeColornSize(ashColor, 4, 0.0f, 0.047f);
	}

#ifndef _DEBUG
	spawnObjects();
#endif
	count = 0;
	prevAccelForce = Vector3(0, 0, 0);
	accelForce = Vector3(0, 0, 0);
}

PlayingGameState::~PlayingGameState()
{
	actorManager.reset();
	delete this->cameraObject;
}

void  PlayingGameState::ImGui_Driving()
{
	ImGui::Begin("OilSpillage");
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Time Left: %f", time);
	ImGui::Text("%s", ramUsage(false).c_str());
	ImGui::Text("%s", vramUsage(false).c_str());
	ImGui::Text(("Rotation: " + std::to_string(player->getRotator())).c_str());
	ImGui::Text(("Angle between: " + std::to_string(player->getAngleBetween())).c_str());
	


	Vector3 camPos = camera->getPosition();
	Vector3 camRot = camera->getRotation();
	Vector2 lDir = Input::getDirectionL();
	Vector2 rDir = Input::getDirectionR();
	float lStr = Input::getStrengthL();
	float rStr = Input::getStrengthR();
	bool status[4] = { Input::checkButton(Keys::CONFIRM, States::UP), Input::checkButton(Keys::CONFIRM, States::HELD), Input::checkButton(Keys::CONFIRM, States::RELEASED), Input::checkButton(Keys::CONFIRM, States::PRESSED) };
	ImGui::Text(("Cam Pos: " + std::to_string(camPos.x) + " " + std::to_string(camPos.y) + " " + std::to_string(camPos.z)).c_str());
	ImGui::Text(("Cam Rot: " + std::to_string(camRot.x) + " " + std::to_string(camRot.y) + " " + std::to_string(camRot.z)).c_str());
	ImGui::Text(("\nConfirm Status - Up: " + std::to_string(status[0]) + " Held: " + std::to_string(status[1]) + " Released: " + std::to_string(status[2]) + " Pressed: " + std::to_string(status[3])).c_str());
	ImGui::Text(("L Dir: " + std::to_string(lDir.x) + " " + std::to_string(lDir.y)).c_str());
	ImGui::Text(("L Str: " + std::to_string(lStr)).c_str());
	ImGui::Text(("R Dir: " + std::to_string(rDir.x) + " " + std::to_string(rDir.y)).c_str());
	ImGui::Text(("R Str: " + std::to_string(rStr)).c_str());
	ImGui::Text(("Accelerator: " + std::to_string(player->getAcceleratorX())).c_str());
	ImGui::Text(("nr of enemies: " + std::to_string(nrOfEnemies)).c_str());
	ImGui::End();
}

void PlayingGameState::ImGui_AI()
{
	ImGui::Begin("AI");
	ImGui::SetWindowSize({ 400,150 });
	ImGui::End();
}

void PlayingGameState::ImGui_Particles()
{
	ImGui::Begin("Particle");


	if (ImGui::BeginCombo("##ParticleSystems", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int i = 0; i < this->graphics.getParticleHandler()->getNames().size(); i++)
		{
			bool is_selected = (current_item == this->graphics.getParticleHandler()->getNames().at(i).c_str()); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(this->graphics.getParticleHandler()->getNames().at(i).c_str(), is_selected))
			{
				current_item = this->graphics.getParticleHandler()->getNames().at(i).c_str();
				this->graphics.setTestParticleSystem(this->graphics.getParticleSystem(current_item));
				this->fillTestParticle();
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}

		}
		ImGui::EndCombo();
	}
	ImGui::ColorPicker4("Color Slider", colors);
	ImGui::ColorPicker4("Color 1 Slider", colors2);
	ImGui::ColorPicker4("Color 2 Slider", colors3);
	ImGui::ColorPicker4("Color 3 Slider", colors4);
	ImGui::SliderFloat("First size", &size1, 0.0f, 1.0f);
	ImGui::SliderFloat("Second size", &size2, 0.0f, 1.0f);
	ImGui::SliderFloat("Init.w (size for electro)", &elSize, 0.0f, 20.0f);
	ImGui::SliderFloat("Init.z (spacing for electro)", &elSpacing, 0.0f, 1.0f);
	ImGui::SliderInt("Nr of particles times 8", &addNrOfParticles, 1, 10);
	ImGui::SliderFloat("LifeTime", &lifeTime, 0.0f, 20.0f);
	ImGui::SliderFloat("Vectorfield size", &vectorFieldSize, 0.0f, 10.0f);
	ImGui::SliderFloat("Vectorfield power", &vectorFieldPower, 0.0f, 10.0f);
	ImGui::SliderFloat("Random power", &randomPosPower, 0.0f, 10.0f);

	colorsP[0] = Vector4(colors);
	colorsP[1] = Vector4(colors2);
	colorsP[2] = Vector4(colors3);
	colorsP[3] = Vector4(colors4);
	colorsP[0].w = 1.0f;
	colorsP[1].w = 1.0f;
	colorsP[2].w = 1.0f;
	colorsP[3].w = 1.0f;
	graphics.setTestVectorField(vectorFieldSize, vectorFieldPower);
	graphics.setTestColorNSize(colorsP, 4, size1, size2);
	graphics.saveTestParticleSystem();
	ImGui::End();
}

#include "../PG/MinimapTextureGenerator.hpp"
void PlayingGameState::ImGui_ProcGen()
{
	ImGui::Begin("Map Generation:");
	if (static bool isFirstFrame = true;  isFirstFrame) {
		ImGui::SetWindowPos({ 0,  75 });
		ImGui::SetWindowSize({ 525, 475 });
		isFirstFrame = false;
	}

	ImGui::Separator();

	// map:
	ImGui::InputInt2("TileMap dimensions", &config.dimensions.data[0]);
	if (config.dimensions.x < 1)
		config.dimensions.x = 1;
	if (config.dimensions.y < 1)
		config.dimensions.y = 1;

	// (TODO! bugged!) ImGui::InputFloat3( "Tile scale", &config.tileScaleFactor.data[0] );

	//ImGui::InputFloat("Floor height factor", &config.buildingFloorHeightFactor);

	ImGui::InputInt("Seed", &config.seed);

	ImGui::Checkbox("Use Manhattan distance", &config.isUsingManhattanDistance); // TODO: refactor into config

	ImGui::InputInt("District cell side", &config.districtCellSide);
	static auto cellSidePrev = config.districtCellSide;
	int stepsCounter = 0;
	auto constexpr MAX_STEPS = 128;
	if (config.districtCellSide < 1)
		config.districtCellSide = 1;
	else if ((config.districtCellSide - cellSidePrev) < 0)
		while ((config.districtCellSide > 1)
			and ((config.dimensions.x % config.districtCellSide) != 0)
			and ((config.dimensions.y % config.districtCellSide) != 0)
			and (++stepsCounter < MAX_STEPS))
			--config.districtCellSide;
	else if ((config.districtCellSide - cellSidePrev) > 0)
		while (((config.dimensions.x % config.districtCellSide) != 0)
			and ((config.dimensions.y % config.districtCellSide) != 0)
			and (++stepsCounter < MAX_STEPS))
			++config.districtCellSide;
	if (stepsCounter == MAX_STEPS) // no match found
		config.districtCellSide = cellSidePrev;
	cellSidePrev = config.districtCellSide;

	ImGui::Separator();

	// roads:

	ImGui::InputInt("Max depth", &config.roadDepthMax);
	if (config.roadDepthMax > 4) // TODO: magic4
		config.roadDepthMax = 4;
	else if (config.roadDepthMax < 1)
		config.roadDepthMax = 1;

	ImGui::SliderFloat("Base min length factor", &config.roadLengthFactorMin, .0f, 1.0f );
	if (config.roadLengthFactorMin > config.roadLengthFactorMax)
		config.roadLengthFactorMin = config.roadLengthFactorMax;

	ImGui::SliderFloat("Base max length factor", &config.roadLengthFactorMax, .0f, 1.0f );
	if (config.roadLengthFactorMax < config.roadLengthFactorMin)
		config.roadLengthFactorMax = config.roadLengthFactorMin;


	ImGui::SliderFloat("Child length factor", &config.roadLengthFactor, .001f, 10.0f);


	ImGui::SliderFloat("Base turn probability", &config.roadTurnProbability, .001f, 1.0f);
	ImGui::SliderFloat("Child turn% factor", &config.roadTurnProbabilityFactor, .001f, 10.0f);


	ImGui::SliderFloat("Base branch probability", &config.roadBranchProbability, .001f, 1.0f);
	ImGui::SliderFloat("Child branch% factor", &config.roadBranchProbabilityFactor, .001f, 10.0f);


	ImGui::InputInt("Min tiles before turn", &config.roadMinTilesBeforeTurn);
	if (config.roadMinTilesBeforeTurn < 0)
		config.roadMinTilesBeforeTurn = 0;

	ImGui::InputInt("Min tiles before branch", &config.roadMinTilesBeforeBranch);
	if (config.roadMinTilesBeforeBranch < 0)
		config.roadMinTilesBeforeBranch = 0;

	ImGui::InputInt("Road step size", &config.roadStepSize);
	if (config.roadStepSize < 1)
		config.roadStepSize = 1;

	// regen button:
	ImGui::NewLine();
	if (ImGui::Button("Re-generate")) {
		// (TODO: refactor) hacky, but:
		player->getRigidBody()->setLinearFactor(btVector3(.0, .0f, .0f));

		map = nullptr; // clear, then regenerate:
		map = std::make_unique<Map>(graphics, config, physics.get(), *lightList);

		player->setPosition(map->getStartPositionInWorldSpace());
		player->getVehicleBody1()->setPosition(map->getStartPositionInWorldSpace() + Vector3(.0f, .55f, .0f));
		minimap = createMinimapTexture(*map);
		generateMapPowerUps();
		createFogOfWarTexture(*map);
		initAI();
		aStar->generateTileData(map->getTileMap());
		// minimap stuff
		auto tilemap = map->getTileMap();
		topLeft = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileSideScaleFactor / 2, 0, config.tileSideScaleFactor / 2);
		bottomRight = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileSideScaleFactor / 2, 0, -config.tileSideScaleFactor / 2);

		graphics.reloadTexture(minimap);
		static_cast<UIPlaying*>(menues[MENU_PLAYING].get())->resetMinimapFog();
		player->getRigidBody()->setLinearFactor(btVector3(1.0f, .0f, 1.0f));

		clearPowerUps();
		generateMapPowerUps();
	}
	ImGui::End();
}

void  PlayingGameState::ImGui_Camera() {
	ImGui::Begin("Camera & Culling:");
	if (static bool firstFrame = true; firstFrame) {
		ImGui::SetWindowPos({ 0,0 });
		ImGui::SetWindowSize({ 425,75 });
		firstFrame = false;
	}
	// camera distance:
	ImGui::SliderFloat("Camera height", &cameraDistance, 2.5f, 1000.0f, "%4.1f");
	// culling distance:
	static float cullingDistanceCurr = graphics.getCullingDistance();
	float const  cullingDistancePrev = cullingDistanceCurr;
	cullingDistanceCurr = graphics.getCullingDistance();
	ImGui::SliderFloat("Culling distance", &cullingDistanceCurr, .0f, 1500.0f, "%4.1f");
	if (cullingDistanceCurr != cullingDistancePrev)
		graphics.setCullingDistance(cullingDistanceCurr);
	ImGui::End();
}

Vector3 PlayingGameState::getRespawnPosition() const noexcept
{
	auto ui = static_cast<UIPlaying const*>(menues[Menu::MENU_PLAYING].get());
	auto maybeHospitalTilePos = map->getNearestFoundHospitalTilePos(player->getPosition(), *ui);
	if (maybeHospitalTilePos) {
		return map->getHospitalFrontPosition(maybeHospitalTilePos.value()) + Vector3(0, -1.2f, 0);
		// TODO: rotate player
	}
	else return map->getStartPositionInWorldSpace() + Vector3(0, -1.2f, 0);
}

std::unique_ptr<Vehicle>& PlayingGameState::getPlayer()
{
	return this->player;
}

void PlayingGameState::setPlayer(Vehicle* theVehicle)
{
	SpotLight* temp = this->player->getSpotLight();
	this->player.reset(theVehicle);// = std::make_unique<Vehicle>(theVehicle);
	this->player->setSpotLight(temp);
}

void PlayingGameState::update(float deltaTime)
{
	/*-------------------------UPDATING-------------------------*/
	if (currentMenu == PlayingGameState::MENU_PLAYING)
	{
		if (Input::isKeyDown_DEBUG(Keyboard::E)) {
			deltaTime /= 4;
		}

#if defined(_DEBUG) || defined(RELEASE_DEBUG)

#else
		if (time > 0.0f)
		{
			time = std::max(time - deltaTime, 0.0f);
		}
		else if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
		{
			Game::setState(Game::STATE_HIGHSCORE);
		}
#endif // !_DEBUG
		prevAccelForce = Vector3(player->getRigidBody()->getLinearVelocity());
		player->updatePlayer(deltaTime);
		physics->update(deltaTime);

		if (player->isDead()) {
			cameraObject->getRigidBody()->setCollisionFlags(cameraObject->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			cameraTimer = 0;
		}
		else {
			cameraTimer += deltaTime;
			float distance = (Vector2(player->getPosition().x,player->getPosition().z) - Vector2(cameraObject->getPosition().x,cameraObject->getPosition().z)).Length();
			if (cameraTimer > 10.5f || distance < 2.5f ) {
				cameraObject->getRigidBody()->setCollisionFlags(cameraObject->getRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
				cameraTimer = 0;
			}
		}
		Vector3 cameraMovement(player->getCameraDistance(deltaTime));
		btVector3 positionCam{ player->getRigidBody()->getWorldTransform().getOrigin() };
		Vector3 destinationCamPos = Vector3(positionCam.getX(),
			positionCam.getY() / 3,
			positionCam.getZ()) + Vector3(cameraMovement.x, cameraMovement.y + cameraDistance, cameraMovement.z);
		Vector3 currentCamPos = Vector3(cameraObject->getRigidBody()->getWorldTransform().getOrigin().getX(), cameraObject->getRigidBody()->getWorldTransform().getOrigin().getY(), cameraObject->getRigidBody()->getWorldTransform().getOrigin().getZ());
		Vector3 directionCam =  destinationCamPos- currentCamPos;
		//cameraObject->getRigidBody()->applyForce(btVector3(directionCam.x, directionCam.y, directionCam.z) * 10,btVector3(0,0,0));
		cameraObject->getRigidBody()->setLinearVelocity(btVector3(std::clamp(directionCam.x,-20.0f,20.0f), std::clamp(directionCam.y, -20.0f, 20.0f), std::clamp(directionCam.z, -20.0f, 20.0f)) * 5);
		camera->setPosition(Vector3(currentCamPos.x, currentCamPos.y, currentCamPos.z));
		camera->update(deltaTime);
		updateWeather(deltaTime, currentCamPos);

		//Vector3 tempCamPos = camera->getPosition() * Vector3(1.0f, 0.0f, 1.0f) + Vector3(0.0f, positionCam.getY() / 3 + cameraMovement.y, 0.0f);
		//Sound::updateListener(tempCamPos, tempCamPos + Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), prevAccelForce);

		size_t playerBulletCount;
		Bullet* playerBullets = player->getBulletArray(playerBulletCount);



		for (std::unique_ptr<PowerUp>& p : powerUps)
		{
			p->update(deltaTime, player->getPosition());
			if (p->isActive() && !player->isDead())
			{
				if (p->getAABB().intersectXZ(player->getAABB()))
				{
					player->addPowerUp(p->getPowerUpType());
					p->deactivate();
				}
			}
		}

		if (time == 0)
		{
			deltaTime /= 4;
			this->player->setHealth(0);
		}

		actorManager->intersectPlayerBullets(playerBullets, playerBulletCount, deltaTime);
		actorManager->update(deltaTime, player->getPosition());
		accelForce = Vector3(player->getRigidBody()->getLinearVelocity().getX(), player->getRigidBody()->getLinearVelocity().getY(), player->getRigidBody()->getLinearVelocity().getZ()) - Vector3(prevAccelForce.x, prevAccelForce.y, prevAccelForce.z);
		player->setAccelForce(accelForce, deltaTime);
		player->setWheelRotation(deltaTime);
		objectives.update(player->getPosition(), physics.get());
		Bullet::updateSoundTimer(deltaTime);
		player->updateWeapon(deltaTime);
#ifdef _DEBUG
		timer += deltaTime;

		if (Input::checkButton(Keys::R_LEFT, States::PRESSED))
		{
			timerEMP = 4.0f;
		}
		if (timerEMP > 0.0f)
		{
			timerEMP -= deltaTime;
		}
		if (timer > 0.1f&&timerEMP>0.0f)
		{
			timer = 0.0f;
			this->graphics.addTestParticle(this->player->getPosition() + Vector3(0, 1, 0), Vector4(0, 0, elSpacing, elSize), addNrOfParticles, lifeTime, randomPosPower);
		}
#endif


#ifndef _DEBUG
		updateObjects();
		paperCollision(deltaTime);
#endif



		btVector3 spotlightDir{ 0,
								 0,
								 1 };
		player->getRotationQuaternion();
		btQuaternion bt5 = btQuaternion(this->player->getVehicleBody1()->getRigidBody()->getWorldTransform().getRotation());
		btTransform transform(bt5, btVector3(0, 0, 0));
		spotlightDir = transform * spotlightDir;
		playerLight->setDirection(Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()));

		Vector3 spotlightPos{ player->getPosition().x,
							   player->getPosition().y + 1,
							   player->getPosition().z };

		spotlightPos += Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()) * 1;

		playerLight->setPos(spotlightPos);

		if (actorManager->distanceToPlayer(Vector3(positionCam)) < 25.0f || this->time <= 20.0f)
		{
			soundAggro = min(Game::lerp(soundAggro, 1.0f, deltaTime * 2), 1.0f);
		}
		else
		{
			soundAggro = std::max(Game::lerp(soundAggro, 0.0f, deltaTime * 2), 0.0f);
		}

		if (this->objectives.isAllDone())
		{
			Sound::stopAllLoops();
			this->setCurrentMenu(MENU_COMPLETED_STAGE);
		}
	}

	/*-------------------------RENDERING-------------------------*/
	// render all objects

	graphics.clearScreen(Vector4(0, 0, 0, 1));

	if (currentMenu != MENU_BEFORE_PLAYING)
	{
		graphics.setSpotLightShadow(playerLight);
		graphics.render(camera.get(), deltaTime);
	}

	// render UI
	if (currentMenu != MENU_PLAYING)
	{
		soundAggro = std::max(Game::lerp(soundAggro, 0.0f, deltaTime * 8), 0.0f);

		if (currentMenu != MENU_BEFORE_PLAYING)
		{
			menues[MENU_PLAYING]->update(0);
		}

		menues[currentMenu]->update(deltaTime);
	}
	else
	{
		menues[MENU_PLAYING]->update(deltaTime);

		if (Input::checkButton(Keys::MENU, States::PRESSED))
		{
			setCurrentMenu(PlayingGameState::MENU_PAUSED);
		}
	}

	if (oldMenu == MENU_CONTROLS)
	{
		menues[MENU_CONTROLS] = std::make_unique<UIControls>();
	}

	oldMenu = -1;

	Sound::fadeSoundtrack(soundAggro);
	
	#if defined(_DEBUG) || defined(RELEASE_DEBUG) //Set RELEASE_DEBUG to false to deactivate imgui in release!
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui_Driving();
		//ImGui_ProcGen();
		//ImGui_AI();
		//ImGui_Particles();
		//ImGui_Camera();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	#endif // !_DEBUG
	graphics.presentScene();
}

F32 const& PlayingGameState::getTimeRef() const noexcept {
	return time;
}

F32 PlayingGameState::getTime() const noexcept {
	return time;
}

void PlayingGameState::setTime(float time) noexcept {
	time = std::max(time, .0f);
}

void PlayingGameState::changeTime(float timeDiff) noexcept {
	time = std::max(time + timeDiff, .0f);
	static_cast<UIPlaying*>(menues[MENU_PLAYING].get())->addTimeChangeText(timeDiff);
}

void PlayingGameState::setCurrentMenu(Menu menu) {
	oldMenu = currentMenu;
	currentMenu = static_cast<int>(menu);

	if (menu == Menu::MENU_OPTIONS || menu == Menu::MENU_COMPLETED_STAGE || menu == Menu::MENU_BEFORE_PLAYING || menu == Menu::MENU_CONTROLS) { 
		menues[currentMenu]->init();
	}
}

Map::Info PlayingGameState::getMapInfo() const
{
	return this->map->getInfo();
}

Vehicle* PlayingGameState::getPlayer() const {
	return player.get();
}

std::string PlayingGameState::getMinimap() const
{
	return minimap;
}

Vector3 PlayingGameState::getTopLeft() const
{
	return topLeft;
}

Vector3 PlayingGameState::getBottomRight() const
{
	return bottomRight;
}

void PlayingGameState::spawnObjects()
{
	physicsObjID = 0;
	physicsObjects.reserve(300);
	btRigidBody* tempo2;
	float randomValue;
	for (int i = 0; i < 20; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = graphics.getMeshPointer("Cube");
		objPtr->setTexture(graphics.getTexturePointer("brownPaperCardboard"));
		graphics.addToDraw(objPtr);
		randomValue = (rand() % 3 + 8) * 0.125f;
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setScale(Vector3(0.38f * randomValue, 0.38f * randomValue, 0.38f * randomValue));
	}
	float size = 1.3f * 0.38f;
	randomValue = 0;
	float randomValue2 = 0;
	float randomValue3 = 0;
	for (int i = 0; i < 15; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = graphics.getMeshPointer("Entities/Barrel");
		objPtr->setTexture(graphics.getMaterial("Entities/Barrel").diffuse);
		graphics.addToDraw(objPtr);
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setScale(Vector3(size, size, size));
		randomValue = (rand() % 5) * 0.03f;
		randomValue2 = (rand() % 5) * 0.03f;
		randomValue3 = (rand() % 5) * 0.03f;
		objPtr->setColor(Vector4(randomValue, randomValue2, 0, 1));

		tempo2 = physics->addCylinder(btVector3(-200, 0.2f + i, -200), btVector3(size, size, size), 0.01f);
		objPtr->setRigidBody(tempo2, physics.get());
		objPtr->getRigidBody()->setFriction(0.7f);
		objPtr->getRigidBody()->setActivationState(0);
		objPtr->getRigidBody()->setDeactivationTime(0.1f);
	}
	for (int i = 0; i < 15; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = graphics.getMeshPointer("Entities/Garbage_Bag");
		objPtr->setTexture(graphics.getMaterial("Entities/Garbage_Bag").diffuse);
		graphics.addToDraw(objPtr);
		objPtr->setPosition(Vector3(0, 0, 0));
		randomValue = (rand() % 3 + 8) * 0.125f;
		size = 0.30f * randomValue;
		objPtr->setScale(Vector3(size, size, size));
		tempo2 = physics->addSphere(size, btVector3(0, 0.0f, 0), 0.01f);
		objPtr->setPosition(Vector3(0, 0, 0));
	}
	for (int i = 0; i < 250; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = graphics.getMeshPointer("Entities/Quad");
		graphics.addToDraw(objPtr);
		size = rand() % 3 + 8;
		size *= 0.125f;
		randomValue = randomValue = (rand() % 5) * 0.125f;
		objPtr->setScale(Vector3(0.12f * size, 0.01f, 0.19f * size));
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setColor(Vector4(1 - randomValue, 1 - randomValue, 1 - randomValue, 1));
		objPtr->setSpotShadow(false);
		objPtr->setSunShadow(false);
	}
}

void PlayingGameState::moveObjects()
{
	GameObject* object = physicsObjects.at(physicsObjID).get();
	auto tilemap = map->getTileMap();
	if (object->getPosition().x > (player->getPosition().x + 80) ||
		object->getPosition().x < (player->getPosition().x - 80) ||
		object->getPosition().z >(player->getPosition().z + 50) ||


		object->getPosition().z < (player->getPosition().z - 50)) {
		if (object->getRigidBody() != nullptr) {
			object->getRigidBody()->setActivationState(0);
		}
		int randomValue = rand() % 2 + 1;
		if (randomValue == 1) {
			if (player->getRigidBody()->getLinearVelocity().getZ() > 0) {
				//Top
				randomValue = rand() % 100 + 1 - 50;
				object->setPosition(Vector3(player->getPosition().x + randomValue, -1.5f, player->getPosition().z + 50));
			}
			else {
				//Bottom
				randomValue = rand() % 100 + 1 - 50;
				object->setPosition(Vector3(player->getPosition().x + randomValue, -1.5f, player->getPosition().z - 50));
			}
		}
		else if (randomValue == 2) {
			if (player->getRigidBody()->getLinearVelocity().getX() < 0) {
				//Left
				randomValue = rand() % 50 + 1 - 25;
				object->setPosition(Vector3(player->getPosition().x - 80, -1.5f, player->getPosition().z + randomValue));
			}
			else {
				//Right
				randomValue = rand() % 50 + 1 - 25;
				object->setPosition(Vector3(player->getPosition().x + 80, -1.5f, player->getPosition().z + randomValue));
			}
		}
		randomValue = rand() % 360 + 1;
		int randomValue2 = 0;
		if (object->getRigidBody() != nullptr) {
			randomValue2 = rand() % 2;
		}
		Quaternion qt1 = Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(randomValue2 * 90 * XM_PI / 180, randomValue * XM_PI / 180, 0));
		btQuaternion qt = btQuaternion(qt1.x, qt1.y, qt1.z, qt1.w);
		if (object->getRigidBody() != nullptr) {
			object->getRigidBody()->getWorldTransform().setRotation(qt);
		}
		else {
			object->setRotation(Vector3(randomValue2 * 90 * XM_PI / 180, randomValue * XM_PI / 180, 0));
		}

	}
	physicsObjID++;
	if (physicsObjID >= physicsObjects.size()) {
		physicsObjID = 0;
	}

}

Vector3 PlayingGameState::generateObjectivePos(float minDistance, float maxDistance) noexcept
{

	for (;;) {
		Vector3 position = map->generateRoadPositionInWorldSpace(rng);
		float distance = (position - player->getPosition()).Length();
		if ((distance <= maxDistance) and (distance >= minDistance))
		{
			return position;
		}
	}
	assert(false and "BUG: Shouldn't be possible!");
	return { -1.0f, -1.0f, -1.0f }; //  silences a warning

}

Vector3 PlayingGameState::generateObjectivePos(Vector3 origin, float minDistance, float maxDistance) noexcept
{

	for (;;) {
		Vector3 position = map->generateRoadPositionInWorldSpace(rng);
		float distance = (position - origin).Length();
		if ((distance <= maxDistance) and (distance >= minDistance))
		{
			return position;
		}
	}
	assert(false and "BUG: Shouldn't be possible!");
	return { -1.0f, -1.0f, -1.0f }; //  silences a warning

}

PointLight* PlayingGameState::addLight(PointLight& light)
{
	return this->lightList->addLight(light);
}

void PlayingGameState::removeLight(PointLight* light)
{
	this->lightList->removeLight(light);
}

SpotLight* PlayingGameState::addLight(SpotLight& light)
{
	return this->lightList->addLight(light);
}

void PlayingGameState::removeLight(SpotLight* light)
{
	this->lightList->removeLight(light);
}

LaserLight* PlayingGameState::addLight(LaserLight& light)
{
	return this->lightList->addLight(light);
}

void PlayingGameState::removeLight(LaserLight* light)
{
	this->lightList->removeLight(light);
}

ObjectiveHandler& PlayingGameState::getObjHandler()
{
	return this->objectives;
}

void PlayingGameState::updateObjects()
{
	if (abs(player->getVelocitySpeed()) > 1.0f) {
		moveObjects();
		if (abs(player->getVelocitySpeed()) > 10.0f) {
			moveObjects();
			if (abs(player->getVelocitySpeed()) > 20.0f) {
				moveObjects();
			}
		}
	}
}

void PlayingGameState::paperCollision(float deltaTime)
{

	float randomValue = 0;
	for (auto& obj : physicsObjects) {
		if (obj->getRigidBody() == nullptr) {
			randomValue = rand() % 2;
			if (((player->getPosition() - obj->getPosition()).Length()) < 1.5f && abs(player->getVelocitySpeed()) > 8.0f && randomValue == 1) {
				randomValue = rand() % 20;
				randomValue = std::max(randomValue - 5.0f, 0.0f);
				obj->setVelocity(Vector3(player->getRigidBody()->getLinearVelocity().getX(), randomValue * 0.5f, player->getRigidBody()->getLinearVelocity().getZ()));
				obj->setPosition(obj->getPosition() + Vector3(0, 0.1f, 0));
			}
			obj->updateObject(deltaTime);
		}
	}
}

void PlayingGameState::addPowerUp(PowerUp p)
{
	powerUps.push_back(std::make_unique<PowerUp>(p));
	powerUps[powerUps.size() - 1]->activate();
}

void PlayingGameState::clearPowerUps()
{
	powerUps.clear();
}

void PlayingGameState::generateMapPowerUps()
{
	UINT index = rng() % 10;
	while (index < map->config.dimensions.x * map->config.dimensions.y)
	{
		Vector3 position = map->generateRoadPositionInWorldSpace(rng);
		Vector3 indexWorldSpace = Vector3(
			(float)(index / map->config.dimensions.y) * map->config.tileSideScaleFactor,
			0.0,
			-(float)(index % map->config.dimensions.x) * map->config.tileSideScaleFactor
		);

		for (int i = 0; i < 100; ++i)
		{
			if (Vector3::Distance(position, indexWorldSpace) > 30.0)
			{
				position = map->generateRoadPositionInWorldSpace(rng);
				indexWorldSpace = Vector3(
					(float)(index / map->config.dimensions.y) * map->config.tileSideScaleFactor,
					0.0,
					-(float)(index % map->config.dimensions.x) * map->config.tileSideScaleFactor
				);
				break;
			}
		}
		position.y += 2.0;
		PowerUp p(position, (PowerUpType)(rng() % (UINT)PowerUpType::Length), 90.f);

		addPowerUp(p);

		index += rng() % 142 + 124;
	}
}

void PlayingGameState::generateObjectives()
{
	if (Game::getNrOfStagesDone() % 3 == 0)
	{
		//difficulty scale
		float scalingNr = Game::getNrOfStagesDone() / 3; // /3
		if (scalingNr == 0)
			scalingNr = 1;

		this->objectives.addObjective(TypeOfMission::BossEvent, 200, 1, "Kill the boss",TypeOfTarget::Size,Vector3(0.0f),nullptr,actorManager->createBoss(this->player->getPosition().x, this->player->getPosition().z, 1, scalingNr)); //fix pos
		this->objectives.addObjective(TypeOfMission::GetToPoint, 0, 1, "Get out", TypeOfTarget::Size, map->getStartPositionInWorldSpace());
	}
	else
	{
		float prob[2] = { 0.5f, 0.5f };
		for (int i = 0; i < 1; i++)
		{
			int dice = rand() % 10 + 1;
			if (dice <= prob[0] * 10)
			{
				objectives.addObjective(TypeOfMission::FindAndCollect, 240, 5*Game::getLocalScale(), "Pick up ", TypeOfTarget::Crate);
				prob[0] -= 0.1f;
				prob[1] += 0.1f;
			}
			else if (dice > (1 - prob[1]) * 10)
			{
				objectives.addObjective(TypeOfMission::KillingSpree, 120, 100*Game::getLocalScale(), "Kill enemies");
				prob[0] += 0.1f;
				prob[1] -= 0.1f;
			}
			prob[0] = std::fmaxf(prob[0], 0.0f);
			prob[1] = std::fmaxf(prob[1], 0.0f);
			prob[0] = std::fminf(prob[0], 1.0f);
			prob[1] = std::fminf(prob[1], 1.0f);
		}
		this->objectives.addObjective(TypeOfMission::GetToPoint, 0, 1, "Get out", TypeOfTarget::Size, player->getPosition());// ->getStartPositionInWorldSpace());
	}
}

void PlayingGameState::updateWeather(float deltaTime, Vector3 cameraPos)
{
	if (map->getInfo().environment.getWeather() == Weather::snow) {
		snowTimer += deltaTime;
		if (snowTimer > 0.05f) {
			for (int i = 0; i < std::clamp((int)player->getVelocitySpeed() * 2, 25, 50); i++) {
				Game::getGraphics().addParticle("snow", 1, 5,
					Vector3(cameraPos.x, cameraPos.y, cameraPos.z) +
					Vector3(0, -10.0f, 0) +
					Vector3(player->getRigidBody()->getLinearVelocity().getX(),
						player->getRigidBody()->getLinearVelocity().getY(),
						player->getRigidBody()->getLinearVelocity().getZ()) * 3.0f,
					Vector4(0.0f, 0.0f, 0.0f, 0.0f),
					20.5f + cameraPos.y,
					Vector3(1, 0, 1));
			}
			snowTimer = 0;
		}
	}
	if (map->getInfo().environment.getWeather() == Weather::rain || map->getInfo().environment.getWeather() == Weather::thunderstorm) {
		snowTimer += deltaTime;
		if (snowTimer > 0.05f) {
			for (int i = 0; i < std::clamp((int)player->getVelocitySpeed() * 2, 25, 50); i++) {
				Game::getGraphics().addParticle("snow", 1, 2,
					Vector3(cameraPos.x, cameraPos.y, cameraPos.z) +
					Vector3(0, -10.0f, 0) +
					Vector3(player->getRigidBody()->getLinearVelocity().getX(),
						player->getRigidBody()->getLinearVelocity().getY(),
						player->getRigidBody()->getLinearVelocity().getZ()) * 3.0f,
					Vector4(0.0f, 0.0f, 0.0f, 0.0f),
					20.5f + cameraPos.y,
					Vector3(1, 0, 1));
			}
			snowTimer = 0;
		}
	}
	if (map->getInfo().environment.getWeather() == Weather::ashfall) {
		snowTimer += deltaTime;
		if (snowTimer > 0.05f) {
			for (int i = 0; i < std::clamp((int)player->getVelocitySpeed() * 2, 25, 50); i++) {
				Game::getGraphics().addParticle("snow", 1, 7,
					Vector3(cameraPos.x, cameraPos.y, cameraPos.z) +
					Vector3(0, -10.0f, 0) +
					Vector3(player->getRigidBody()->getLinearVelocity().getX(),
						player->getRigidBody()->getLinearVelocity().getY(),
						player->getRigidBody()->getLinearVelocity().getZ()) * 3.0f,
					Vector4(0.0f, 0.0f, 0.0f, 0.0f),
					20.5f + cameraPos.y,
					Vector3(1, 0, 1));
			}
			snowTimer = 0;
		}
	}
}

Vector3 PlayingGameState::getCameraPos()
{
	return this->camera->getPosition();
}

