#include "PlayingGameState.h"
#include "../Input.h"
#include "../Sound.h"
#include "../UI/Playing/UIPlaying.h"
#include "../UI/Playing/UIPaused.h"
#include "../UI/Menu/UIOptions.h"
#include "../PG/MinimapTextureGenerator.hpp"
#include "../PG/Profiler.hpp"

void PlayingGameState::initAI()
{
	aStar = new AStar( map->getTileMap() );
	actorManager = new ActorManager(aStar);
	aStar->generateTileData(map->getTileMap());
}

PlayingGameState::PlayingGameState() : graphics(Game::getGraphics()), time(360.0f), currentMenu(MENU_PLAYING)
{
   #if defined(_DEBUG) || defined(RELEASE_DEBUG)
	   pausedTime = false;
   #endif // _DEBUG


	rng.seed(config.seed); // gör i konstruktorn
	lightList = std::make_unique<LightList>();
	camera = std::make_unique<DynamicCamera>();
	//testNetwork = std::make_unique<RoadNetwork>(2430, Vector2(16.0f, 16.0f), Vector2(-16.0f,-16.0f), 25); //Int seed, max pos, min pos, angle in degrees
	graphics.createFrustumBuffer(camera.get());


	graphics.loadMesh("Cube");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall");
	graphics.loadTexture("grass3");

	graphics.loadTexture("brickwallnormal");
	graphics.loadModel("Dummy_Roller_Melee");
	graphics.loadModel("Entities/Dummy_Turret");
	graphics.loadModel("Entities/Dummy_Player_Car", Vector3(3.14f / 2, 0, 0));

	graphics.loadModel("Roads/Metro/0000");
	graphics.loadModel("Roads/Metro/0001");
	graphics.loadModel("Roads/Metro/0010");
	graphics.loadModel("Roads/Metro/0011");
	graphics.loadModel("Roads/Metro/0100");
	graphics.loadModel("Roads/Metro/0101");
	graphics.loadModel("Roads/Metro/0110");
	graphics.loadModel("Roads/Metro/0111");
	graphics.loadModel("Roads/Metro/1000");
	graphics.loadModel("Roads/Metro/1001");
	graphics.loadModel("Roads/Metro/1010");
	graphics.loadModel("Roads/Metro/1011");
	graphics.loadModel("Roads/Metro/1100");
	graphics.loadModel("Roads/Metro/1101");
	graphics.loadModel("Roads/Metro/1110");
	graphics.loadModel("Roads/Metro/1111");

	graphics.loadModel("Hospital");
	//graphics.loadModel("Roads/Road_pavement");
	//graphics.loadModel("Roads/Road_deadend");
	//graphics.loadModel("Roads/Road_bend");
	//graphics.loadModel("Roads/Road_straight");
	//graphics.loadModel("Roads/Road_3way");
	//graphics.loadModel("Roads/Road_4way");
	graphics.loadModel("Houses/testHouse");
	graphics.loadModel("Houses/testHouse2");
	graphics.loadModel("Houses/testHouse3");
	graphics.loadModel("Houses/testHouse4");
	graphics.loadModel("Houses/testHouse5");
	graphics.loadModel("Houses/houseMaterial");
	graphics.loadModel("Houses/houseMaterial2");


	player = std::make_unique<Vehicle>();

   //if constexpr ( isDebugging ) {
	   // light tests
	   lightList->addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 1.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 1.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 1.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

		lightList->removeLight(lightList->addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));

	for (int i = 0; i < 10; ++i) {
		Vector3 randPos   = Vector3(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 9 + 1), -static_cast<float>(rand() % 1000));
		Vector3 randColor = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand()))/ RAND_MAX;
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		lightList->addLight(
			PointLight(
				randPos,
				randColor,
			10.0f));
	}
   
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
   //}
	lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));

	graphics.setLightList(lightList.get());
	SpotLight tempLight(Vector3(0, 0, 0), Vector3(0.9, 0.5, 0), 1.0f, Vector3(0, 0, 0), 0.4f);
	this->player->setSpotLight(lightList->addLight(tempLight));
	physics = std::make_unique<Physics>();
	player->init(physics.get());
	
	map = std::make_unique<Map>(graphics, config, physics.get());
   map->setDistrictColorCoding( isDebugging );
	// Minimap stuff
   auto tilemap = map->getTileMap();
	topLeft      = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileScaleFactor.x, 0, config.tileScaleFactor.z);
	bottomRight  = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileScaleFactor.x, 0, -config.tileScaleFactor.z);
	// Needs to be loaded before the menues
	minimap = createMinimapTexture(*map);

	menues[MENU_PLAYING] = std::make_unique<UIPlaying>();
	menues[MENU_PLAYING]->init();
	menues[MENU_PAUSED] = std::make_unique<UIPaused>();
	menues[MENU_PAUSED]->init();
	menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	menues[MENU_OPTIONS]->init();

	auto startPos = map->getStartPositionInWorldSpace();
   auto playerVehicle = player->getVehicle();
	playerVehicle->setPosition(				startPos + Vector3( .0f, 0.00f -1.2f, .0f) );
	player->getVehicleBody1()->setPosition(	startPos + Vector3( .0f, 0.65f -1.2f, .0f) );

	initAI();



	Game::getGraphics().setUISun(Vector3(1.0f, -1.0f, 0.1f), Vector4(0.84, 0.83, 0, 1));

	playerLight = lightList->addLight(SpotLight(playerVehicle->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 2.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	points = {
		{
		 Vector3(0.0f, 30.0f + cameraDistance, -10.0f) + playerVehicle->getPosition(),
		 Vector3(0.0f, 0.0f, 0.0f), 0.0f },
		{
		 Vector3(0.0f, cameraDistance, 0.0f) + playerVehicle->getPosition(),
		 Vector3(XM_PIDIV2, 0.0f, 0.0f), 3.0f }
	};
	camera->startCinematic(&points, false);
	Vector4 colorP2[] = {
		Vector4(0.03f,0.03f,0.03f,1),
		Vector4(0.9f, 0.9f, 0.05f, 1)
	};

	graphics.setParticleColorNSize(colorsP, 4, size1, size2);
	graphics.setParticle2ColorNSize(colorP2, 2, 0.025f, 0.05f);

	graphics.setVectorField(4.5f, 3.0f);

	powerUps.push_back(PowerUp(Vector3(100, 0.0, -100), PowerUpType::Speed));
	Game::getGraphics().addToDraw(&*powerUps.begin());

	objectives.addObjective(TypeOfMission::KillingSpree, 120, 20, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 5, "Pick up the important" ,TypeOfTarget::Crate);


	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::KillingSpree, 240, 75, "Kill the enemies");
	objectives.addObjective(TypeOfMission::KillingSpree, 120, 50, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 6, "Pick up the important");
	objectives.addObjective(TypeOfMission::KillingSpree, 120, 25, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 8, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 7, "Pick up the important");

	//Bullet
	/*buildingTest = std::make_unique<GameObject>();
	Game::getGraphics().loadModel("Vehicles/Dummy_Player_Car");
	buildingTest->mesh = Game::getGraphics().getMeshPointer("Vehicles/Dummy_Player_Car");
	Game::getGraphics().addToDraw(buildingTest.get());
	btRigidBody* tempo2 = physics->addBox(btVector3(-15, 0.0f, -15.0f), btVector3(10.0f, 100.0f, 10.0f), 0.0f);
	buildingTest->setPosition(Vector3(-15, 0.0f, -15.0f));
	buildingTest->setScale(Vector3(10.0f, 100.0f, 10.0f));
	buildingTest->setColor(Vector4(0.5, 0.5, 0.5, 1));
	buildingTest->setRigidBody(tempo2, physics.get());*/

#ifndef _DEBUG
	spawnObjects();
#endif
	count = 0;
	prevAccelForce = Vector3(0,0,0);
	accelForce = Vector3(0, 0, 0);
	soundAggro = 0;
}

PlayingGameState::~PlayingGameState()
{
	delete aStar;
	delete actorManager;


	delete this->objTestPickUp;
	delete this->objTestPickUp2;
	delete this->objTestPickUp3;

	delete[] this->objArray;
}

void  PlayingGameState::ImGui_Driving()
{
	ImGui::Begin("OilSpillage");
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Time Left: %f", time);
	ImGui::Text(("Rotation: " + to_string(player->getRotator())).c_str());
	ImGui::Text("Driving Mode:");
	static int radioButtonValue = 0;
	ImGui::RadioButton("Directional Semi-Realistic", &radioButtonValue, 0);
	ImGui::RadioButton("Realistic", &radioButtonValue, 1);
	if (radioButtonValue == 0)
		player->setDrivingMode(1);
	else if (radioButtonValue == 1)
		player->setDrivingMode(0);


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
	ImGui::Text(("Groups: " + std::to_string(actorManager->groups.size())).c_str());
	ImGui::End();
}

void PlayingGameState::ImGui_AI()
{
	ImGui::Begin("AI");
	ImGui::SetWindowSize({ 400,150 });

	//ImGui::Text(("Groups: " + std::to_string(actorManager->groups.size())).c_str());
	//for (int i = 0; i < actorManager->groups.size(); i++)
	//{
	//	ImGui::Text(("Group " + std::to_string(i) + ":" + std::to_string(actorManager->groups.at(i).size())).c_str());
	//}
	//ImGui::Text(("x: " + std::to_string(player->getVehicle()->getPosition().x)).c_str());
	//ImGui::Text(("y: " + std::to_string(player->getVehicle()->getPosition().y)).c_str());
	//ImGui::Text(("z: " + std::to_string(player->getVehicle()->getPosition().z)).c_str());

	//Vector3 xzPos = Vector3(player->getVehicle()->getPosition().x, 0, -player->getVehicle()->getPosition().z);

	//ImGui::Text(("Tile x: " + std::to_string(map->getTileMap().convertWorldPositionToTilePositionXZ(xzPos).x)).c_str());
	//ImGui::Text(("Tile y: " + std::to_string(map->getTileMap().convertWorldPositionToTilePositionXZ(xzPos).y)).c_str());
	/*	+ std::to_string(player->getVehicle()->getPosition().y).c_str()
							+ std::to_string(player->getVehicle()->getPosition().z).c_str()));*/



	//if(actorManager->groups.size() != 0)
	//{
	//	std::vector<Actor*>* temp = actorManager->findClosestGroup(player->getVehicle()->getPosition());
	//	for (int i = 0; i < temp->size(); i++)
	//	{
	//		ImGui::Text(("AI nr " + to_string(i) + ": " + to_string(temp->at(i)->getPosition().x) + " " + to_string(temp->at(i)->getPosition().y) + " " + to_string(temp->at(i)->getPosition().z) + " ").c_str());
	//	}
	//}

	//delete temp;
	ImGui::End();
}

void PlayingGameState::ImGui_Particles()
{
	ImGui::Begin("Particle");
	ImGui::ColorPicker4("Color Slider", colors);
	ImGui::ColorPicker4("Color 1 Slider", colors2);
	ImGui::ColorPicker4("Color 2 Slider", colors3);
	ImGui::ColorPicker4("Color 3 Slider", colors4);
	ImGui::SliderFloat("First size", &size1, 0.0f, 1.0f);
	ImGui::SliderFloat("Second size", &size2, 0.0f, 1.0f);
	ImGui::SliderInt("Nr of particles times 8", &addNrOfParticles, 1, 10);
	ImGui::SliderInt("LifeTime", &lifeTime, 1, 20);
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
	graphics.setVectorField(vectorFieldSize, vectorFieldPower);
	graphics.setParticleColorNSize(colorsP, 4, size1, size2);
	graphics.setParticle2ColorNSize(colorsP, 4, size1, size2);

	ImGui::End();
}

#include "../PG/MinimapTextureGenerator.hpp"
void PlayingGameState::ImGui_ProcGen()
{
   static Bool shouldColorCodeDistricts { true };
	ImGui::Begin( "Map Generation:" );
	if ( static bool isFirstFrame = true;  isFirstFrame ) {
		ImGui::SetWindowPos({    0,  75 });
		ImGui::SetWindowSize({ 525, 475 });
		isFirstFrame = false;
	}

	ImGui::Separator();

	// debug colors toggle:
	bool shouldColorCodeDistrictsPrevFrame = shouldColorCodeDistricts;
	ImGui::Checkbox( "Show district colors", &shouldColorCodeDistricts );
	if ( shouldColorCodeDistricts != shouldColorCodeDistrictsPrevFrame )
		map->setDistrictColorCoding( shouldColorCodeDistricts );

	ImGui::Separator();

	// map:
	ImGui::InputInt2( "TileMap dimensions", &config.dimensions.data[0] );
	if ( config.dimensions.x < 1 )
		config.dimensions.x = 1;
	if ( config.dimensions.y < 1 )
		config.dimensions.y = 1;

	// (TODO! bugged!) ImGui::InputFloat3( "Tile scale", &config.tileScaleFactor.data[0] );

	ImGui::InputFloat( "Floor height factor", &config.buildingFloorHeightFactor );

	ImGui::InputInt( "Seed", &config.seed );

	ImGui::Checkbox( "Use Manhattan distance", &config.isUsingManhattanDistance ); // TODO: refactor into config

	ImGui::InputInt( "District cell side", &config.districtCellSide );
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

	ImGui::InputInt("Base min length", &config.roadLengthMin);
	if (config.roadLengthMin > config.roadLengthMax)
		config.roadLengthMin = config.roadLengthMax;

	ImGui::InputInt("Base max length", &config.roadLengthMax);
	if (config.roadLengthMax < config.roadLengthMin)
		config.roadLengthMax = config.roadLengthMin;

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
		map = std::make_unique<Map>(graphics, config, physics.get());
		player->getVehicle()->setPosition( map->getStartPositionInWorldSpace() );
		map->setDistrictColorCoding( shouldColorCodeDistricts );
		minimap = createMinimapTexture( *map );
		aStar->generateTileData( map->getTileMap() );
		// minimap stuff
      auto tilemap = map->getTileMap();
		topLeft      = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileScaleFactor.x, 0, config.tileScaleFactor.z);
		bottomRight  = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileScaleFactor.x, 0, -config.tileScaleFactor.z);

		graphics.reloadTexture(minimap);
		static_cast<UIPlaying*>(menues[MENU_PLAYING].get())->resetMinimapFog();
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
	auto ui = static_cast<UIPlaying const *>( menues[Menu::MENU_PLAYING].get() );
	auto maybeHospitalTilePos = map->getNearestFoundHospitalTilePos( player->getVehicle()->getPosition(), *ui );
	if ( maybeHospitalTilePos ) {
		return map->getHospitalFrontPosition( maybeHospitalTilePos.value() )+Vector3(0,-1.2f,0);
		// TODO: rotate player
	}
	else return map->getStartPositionInWorldSpace() + Vector3(0, -1.2f, 0);
}

void  PlayingGameState::update(float deltaTime)
{
	/*-------------------------UPDATING-------------------------*/
	if (currentMenu == PlayingGameState::MENU_PLAYING)
	{
		if (Input::isKeyDown_DEBUG(Keyboard::E)) {
			deltaTime /= 4;
		}
		if (Input::checkButton(Keys::CANCEL, States::PRESSED)) {
			if (player->getDrivingMode() == 0) {
				player->setDrivingMode(1);
			}
			else if(player->getDrivingMode() == 1) {
				player->setDrivingMode(0);
			}
		}

#if defined(_DEBUG) || defined(RELEASE_DEBUG)
		if (Input::checkButton(Keys::ACTION_1, States::PRESSED))
		{
			pausedTime = !pausedTime;
		}

		if (!pausedTime && time > 0.0f)
		{
			time = max(time - deltaTime, 0.0f);
		}
		else if (time <= 0.0f)
		{
			Game::setState(Game::STATE_MENU);
		}
#else
		if (time > 0.0f)
		{
			time = max(time - deltaTime, 0.0f);
		}
		else if(Input::checkButton(Keys::CONFIRM,States::PRESSED))
		{
			Game::setState(Game::STATE_MENU);
		}
#endif // !_DEBUG

		//Bullet
		//player->getVehicle()->getRigidBody()->(btVector3(Input::GetDirectionL(0).x * deltaTime * 1000, 0, Input::GetDirectionL(0).y * deltaTime * 1000), btVector3(0, 0, 0));

		//player->getVehicle()->setPosition(Vector3(player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getX(), player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getY(), player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getZ()));
		//player->getVehicle()->updateRigidBody();
		
		auto playerVehicle { player->getVehicle() };
		size_t playerBulletCount;
		Bullet* playerBullets = player->getBulletArray(playerBulletCount);
		
		if(spawnTimer % 200 == 0)
		{
			actorManager->spawnAttackers(generateObjectivePos(50.0f, 100.0f));
			spawnTimer = 0;
		}
		spawnTimer++;

		powerUps.erase(
			std::remove_if(
				powerUps.begin(),
				powerUps.end(),
				[&](PowerUp& p) {
					p.update(time);
					if (p.getAABB().intersectXZ(player->getVehicle()->getAABB()))
					{
						player->powerUp(p.getPowerUpType());
						return true;
					}
					else
					{
						return false;
					}
				}
			),
			powerUps.end()
		);

		if (time == 0)
		{
			deltaTime /= 4;
			this->player->setHealth(0);
		}
		prevAccelForce = Vector3(playerVehicle->getRigidBody()->getLinearVelocity());
		player->update(deltaTime);
		physics->update(deltaTime);
		accelForce = Vector3(player->getVehicle()->getRigidBody()->getLinearVelocity().getX(), player->getVehicle()->getRigidBody()->getLinearVelocity().getY(), player->getVehicle()->getRigidBody()->getLinearVelocity().getZ()) - Vector3(prevAccelForce.x, prevAccelForce.y, prevAccelForce.z);
		player->setAccelForce(accelForce, deltaTime);
		player->setWheelRotation();
		actorManager->update(deltaTime, playerVehicle->getPosition());
		actorManager->intersectPlayerBullets(playerBullets, playerBulletCount);
		camera->update(deltaTime);
		objectives.update(player->getVehicle()->getPosition());
		Bullet::updateSoundTimer(deltaTime);
		player->updateWeapon(deltaTime);
		
#ifndef _DEBUG
		updateObjects();
#endif
		btVector3 positionCam { playerVehicle->getRigidBody()->getWorldTransform().getOrigin() };

		Vector3 cameraMovement(player->getCameraDistance(deltaTime));
		camera->setPosition( Vector3( positionCam.getX(),
		                              positionCam.getY()/3,
		                              positionCam.getZ() ) + Vector3(cameraMovement.x, cameraMovement.y + cameraDistance, cameraMovement.z) );

		btVector3 spotlightDir { 0,
								 0,
		                         1 };
		playerVehicle->getRotationQuaternion();
		btQuaternion bt5 = btQuaternion(this->player->getVehicleBody1()->getRigidBody()->getWorldTransform().getRotation());
		btTransform transform(bt5,btVector3(0, 0,0));
		spotlightDir = transform * spotlightDir;
		playerLight->setDirection( Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()));

		Vector3 spotlightPos { playerVehicle->getPosition().x,
		                       playerVehicle->getPosition().y + 1,
		                       playerVehicle->getPosition().z };

		spotlightPos += Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()) * 1;

		playerLight->setPos( spotlightPos );
		
		if ((actorManager->distanceToPlayer(Vector3(positionCam)) < 40.0f && soundAggro < 1.0f) || this->time <= 20.0f) {
			soundAggro += 0.2f * deltaTime;
		}
		else if(soundAggro > 0.0f) {
			soundAggro -= 0.1f * deltaTime;
		}
		Sound::changeVolume(L"data/sound/OilSpillageSoundtrack1_Aggressive.wav", soundAggro);

		/*timerForParticle += deltaTime;
		if ( timerForParticle > .01f )
		{
			graphics.addParticle( player->getVehicle()->getPosition() + Vector3(0, 5, 0),
			                      5 * Vector3(0,0,0),
			                      addNrOfParticles, lifeTime, randomPosPower);
			timerForParticle = 0;
		}*/
		
	}
	
	/*-------------------------RENDERING-------------------------*/
	// render all objects

	graphics.setSpotLighShadow(playerLight);
	graphics.render( camera.get(), deltaTime );
	
	// render UI
	menues[MENU_PLAYING]->update( deltaTime );
	if ( currentMenu != MENU_PLAYING )
		menues[currentMenu]->update( deltaTime );
	else if ( Input::checkButton(Keys::MENU, States::PRESSED) )
		setCurrentMenu( PlayingGameState::MENU_PAUSED );
	

	//Render all objects
	
	//testNetwork.get()->drawRoadNetwork(&graphics);
	
#if defined(_DEBUG) || defined(RELEASE_DEBUG) //Set RELEASE_DEBUG to false to deactivate imgui in release!
	   /*ImGui_ImplDX11_NewFrame();
	   ImGui_ImplWin32_NewFrame();
	   ImGui::NewFrame();
	   ImGui_Driving();
	   ImGui_ProcGen();
	   ImGui_AI();
	   ImGui_Particles();
	   ImGui_Camera();
	   ImGui::Render();
	   ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/
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
	time = max( time, .0f );
}

void PlayingGameState::changeTime(float timeDiff) noexcept {
	time = max( time + timeDiff, .0f );
}

void PlayingGameState::setCurrentMenu(Menu menu) {
	currentMenu = static_cast<int>(menu);
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
	Game::getGraphics().loadModel("Entities/Barrel");
	Game::getGraphics().loadModel("Entities/Garbage_Bag");
	physicsObjID = 0;
	physicsObjects.reserve(300);
	btRigidBody* tempo2;
	float randomValue;
	for (int i = 0; i < 20; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		Game::getGraphics().loadModel("Cube");
		objPtr->mesh = Game::getGraphics().getMeshPointer("Cube");
		graphics.loadTexture("brownPaperCardboard");
		objPtr->setTexture(Game::getGraphics().getTexturePointer("brownPaperCardboard"));
		Game::getGraphics().addToDraw(objPtr);
		randomValue = (rand() % 3 + 8) * 0.125f;
		tempo2 = physics->addBox(btVector3(-200, 0.2f + i, -200), btVector3(0.38f * randomValue, 0.38f * randomValue, 0.38f * randomValue), 0.01f);
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setScale(Vector3(0.38f * randomValue, 0.38f * randomValue, 0.38f * randomValue));
		//objPtr->setColor(Vector4(0.8, 0.6, 0.38, 1));
		objPtr->setRigidBody(tempo2, physics.get());
		objPtr->getRigidBody()->setFriction(0.7f);
		objPtr->getRigidBody()->setActivationState(0);
		objPtr->getRigidBody()->setDeactivationTime(0.1f);
	}
	float size = 1.3f*0.38f;
		  randomValue  = 0;
	float randomValue2 = 0;
	float randomValue3 = 0;
	for (int i = 0; i < 15; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
		objPtr->setTexture(Game::getGraphics().getMaterial("Entities/Barrel").diffuse);
		Game::getGraphics().addToDraw(objPtr);
		tempo2 = physics->addCylinder(btVector3(-200, 0.2f + i, -200), btVector3(size,size,size), 0.01f);
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setScale(Vector3(size,size,size));
		randomValue = (rand() % 5) * 0.03f;
		randomValue2 = (rand() % 5) * 0.03f;
		randomValue3 = (rand() % 5) * 0.03f;
		objPtr->setColor(Vector4(randomValue, randomValue2, 0, 1));
		objPtr->setRigidBody(tempo2, physics.get());
		objPtr->getRigidBody()->setFriction(0.7f);
		objPtr->getRigidBody()->setActivationState(0);
		objPtr->getRigidBody()->setDeactivationTime(0.1f);
	}
	for (int i = 0; i < 15; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		objPtr->mesh = Game::getGraphics().getMeshPointer("Entities/Garbage_Bag");
		objPtr->setTexture(Game::getGraphics().getMaterial("Entities/Garbage_Bag").diffuse);
		Game::getGraphics().addToDraw(objPtr);
		objPtr->setPosition(Vector3(0, 0, 0));
		randomValue = (rand() % 3 + 8) * 0.125f;
		size = 0.30f *randomValue;
		objPtr->setScale(Vector3(size, size, size));
		tempo2 = physics->addSphere(size, btVector3(0, 0.0f, 0), 0.01f);
		objPtr->setPosition(Vector3(0, 0, 0));
		//objPtr->setColor(Vector4(0.8, 0.6, 0.38, 1));
		objPtr->setRigidBody(tempo2, physics.get());
		objPtr->getRigidBody()->setDamping(0,0.98f);
		objPtr->getRigidBody()->setFriction(0.7f);
		objPtr->getRigidBody()->setActivationState(0);
		objPtr->getRigidBody()->setDeactivationTime(0.1f);
	}
	for (int i = 0; i < 250; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		Game::getGraphics().loadModel("Cube");
		objPtr->mesh = Game::getGraphics().getMeshPointer("Cube");
		Game::getGraphics().addToDraw(objPtr);
		size = rand() % 3 + 8;
		size *= 0.125f;
		randomValue = randomValue = (rand() % 5) * 0.125f;
		objPtr->setScale(Vector3(0.12f*size, 0.01f, 0.19f*size));
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setColor(Vector4(1-randomValue, 1-randomValue, 1-randomValue, 1));
		objPtr->setSpotShadow(false);
		objPtr->setSunShadow(false);
	}

	//physicsObjects.emplace_back(std::make_unique<GameObject>());
	//auto objPtr = physicsObjects.back().get();
	//Game::getGraphics().loadModel("Cube");
	//objPtr->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(objPtr);
	//tempo2 = physics->addBox(btVector3(20, 0.2f, -20), btVector3(5.5f, 12.5f, 5.5f), 0);
	//objPtr->setPosition(Vector3(20, 0.2f, -20));
	//objPtr->setScale(Vector3(5.5f, 12.5f, 5.5f));
	//objPtr->setColor(Vector4(0.3, 0.3, 0.9, 1));
	//objPtr->setRigidBody(tempo2, physics.get());
	//Quaternion qt1 = Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(XM_PI / 3, 0, 0));
	//btQuaternion qt = btQuaternion(qt1.x,qt1.y,qt1.z,qt1.w);
	//objPtr->getRigidBody()->getWorldTransform().setRotation(qt);
	//objPtr->getRigidBody()->setFriction(1);

	//physicsObjects.reserve(2000);
	//auto tilemap = map->getTileMap();
	//for (int tileY = 0; tileY < tilemap.width; ++tileY ) {
	//	for (int tileX = 0; tileX < tilemap.height; ++tileX) {
	//		if (tilemap.tileAt(tileX, tileY) == Tile::road) {
	//			Vector3 min{ tilemap.convertTilePositionToWorldPosition(tileX,tileY) - Vector3 { config.tileScaleFactor.x / 2,
	//																							 config.tileScaleFactor.y / 2,
	//																							 config.tileScaleFactor.x / 2 } };

	//			Vector3 max{ tilemap.convertTilePositionToWorldPosition(tileX,tileY) + Vector3 { config.tileScaleFactor.x / 2,
	//																							 config.tileScaleFactor.y / 2,
	//																							 config.tileScaleFactor.x / 2 } };
	//			/*for (int i = 0; i < 4; i++) {
	//				physicsObjects.emplace_back(std::make_unique<GameObject>());
	//				auto objPtr = physicsObjects.back().get();
	//				Game::getGraphics().loadModel("Cube");
	//				objPtr->mesh = Game::getGraphics().getMeshPointer("Cube");
	//				Game::getGraphics().addToDraw(objPtr);
	//				tempo2 = physics->addBox(btVector3(min.x, 0.2f+i, min.z), btVector3(0.5f, 0.5f, 0.5f), 3.0f);
	//				objPtr->setPosition(Vector3(min.x, 0.2f+i, min.z));
	//				objPtr->setScale(Vector3(0.5f, 0.5f, 0.5f));
	//				objPtr->setColor(Vector4(0.7, 0.7, 0.3, 1));
	//				objPtr->setRigidBody(tempo2, physics.get());
	//				objPtr->getRigidBody()->setFriction(2);
	//				objPtr->getRigidBody()->setActivationState(0);
	//				
	//			}*/
	//		}
	//	}
	//}

}

void PlayingGameState::moveObjects()
{
	GameObject* object = physicsObjects.at(physicsObjID).get();
	auto tilemap = map->getTileMap();
	if (object->getPosition().x > (player->getVehicle()->getPosition().x + 50) ||
		object->getPosition().x < (player->getVehicle()->getPosition().x - 50) ||
		object->getPosition().z > (player->getVehicle()->getPosition().z + 35) ||
		object->getPosition().z < (player->getVehicle()->getPosition().z - 35)) {
		if (object->getRigidBody() != nullptr) {
			object->getRigidBody()->setActivationState(0);
		}
		int randomValue = rand() % 2+1;
		if (randomValue == 1) {
			if (player->getVehicle()->getRigidBody()->getLinearVelocity().getZ() > 0) {
				//Top
				randomValue = rand() % 100 + 1 - 50;
				object->setPosition(Vector3(player->getVehicle()->getPosition().x + randomValue, -1.5f, player->getVehicle()->getPosition().z + 35));
			}
			else {
				//Bottom
				randomValue = rand() % 100 + 1 - 50;
				object->setPosition(Vector3(player->getVehicle()->getPosition().x + randomValue, -1.5f, player->getVehicle()->getPosition().z - 35));
			}
		}else if(randomValue == 2){
			if (player->getVehicle()->getRigidBody()->getLinearVelocity().getX() < 0) {
				//Left
				randomValue = rand() % 50 + 1 - 25;
				object->setPosition(Vector3(player->getVehicle()->getPosition().x - 50, -1.5f, player->getVehicle()->getPosition().z + randomValue));
			}
			else {
				//Right
				randomValue = rand() % 50 + 1 - 25;
				object->setPosition(Vector3(player->getVehicle()->getPosition().x + 50, -1.5f, player->getVehicle()->getPosition().z + randomValue));
			}
		}
		randomValue = rand() % 360 + 1;
		int randomValue2 = 0;
		if (object->getRigidBody() != nullptr) {
			randomValue2 = rand() % 2;
		}
		Quaternion qt1 = Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(randomValue2 * 90 * XM_PI / 180, randomValue * XM_PI/180, 0));
		btQuaternion qt = btQuaternion(qt1.x,qt1.y,qt1.z,qt1.w);
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
		float distance = (position - player->getVehicle()->getPosition()).Length();
		if ( (distance <= maxDistance) and (distance >= minDistance) )
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
		if ( (distance <= maxDistance) and (distance >= minDistance) )
		{
			return position;
		}
	}
	assert(false and "BUG: Shouldn't be possible!");
	return { -1.0f, -1.0f, -1.0f }; //  silences a warning

}

PointLight* PlayingGameState::addPointLight(PointLight& light)
{
	return this->lightList->addLight(light);
}

void PlayingGameState::removeLight(PointLight* theLight)
{
	this->lightList->removeLight(theLight);
}

ObjectiveHandler& PlayingGameState::getObjHandler()
{
	return this->objectives;
}

void PlayingGameState::addTime(float time)
{
	this->time += time;
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
