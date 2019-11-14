#include "PlayingGameState.h"
#include "../Input.h"
#include "../Sound.h"
#include "../UI/UIPlaying.h"
#include "../UI/UIPaused.h"
#include "../UI/UIOptions.h"
#include "../PG/MinimapTextureGenerator.hpp"
#include "../PG/Profiler.hpp"

void PlayingGameState::initAI()
{
	aStar = new AStar(20, 20, Vector2(-10, 10));
	actorManager = new ActorManager(aStar);
	aStar->generateTileData(map->getTileMap());

	//actorManager->createDefender(0 + 1, 0 + 2,
	//	Vector3(0 + 9, 0, 0 + 9));

	//actorManager->createAttacker(map->getStartPositionInWorldSpace().x + 1, map->getStartPositionInWorldSpace().z - 2);
	//actorManager->createAttacker(map->getStartPositionInWorldSpace().x + 1, map->getStartPositionInWorldSpace().z - 4);

	actorManager->createTurret(0 + 2, 0 + 2);
	actorManager->initGroups();

}
PlayingGameState::PlayingGameState() : graphics(Game::getGraphics()), time(125.0f), currentMenu(MENU_PLAYING)
{
   #ifdef _DEBUG
	   pausedTime = false;
   #endif // _DEBUG

	lightList = std::make_unique<LightList>();
	player = std::make_unique<Vehicle>();
	camera = std::make_unique<DynamicCamera>();
	//testNetwork = std::make_unique<RoadNetwork>(2430, Vector2(16.0f, 16.0f), Vector2(-16.0f,-16.0f), 25); //Int seed, max pos, min pos, angle in degrees
	graphics.createFrustumBuffer(camera.get());

	graphics.loadMesh("Cube");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall");
	graphics.loadTexture("testWood");
	graphics.loadTexture("test");
	graphics.loadTexture("fireball");
	graphics.loadTexture("brickwallnormal");
	graphics.loadModel("Dummy_Roller_Melee");
	graphics.loadModel("Entities/Dummy_Turret");
	graphics.loadModel("Entities/Dummy_Player_Car");

	graphics.loadModel("Roads/Road_pavement");
	graphics.loadModel("Roads/Road_deadend");
	graphics.loadModel("Roads/Road_bend");
	graphics.loadModel("Roads/Road_straight");
	graphics.loadModel("Roads/Road_3way");
	graphics.loadModel("Roads/Road_4way");
	graphics.loadModel("Houses/testHouse");

   //if constexpr ( isDebugging ) {
	   // light tests
	   lightList->addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 1.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 1.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 1.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	   lightList->addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	lightList->removeLight(lightList->addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));

	for (int i = 0; i < 1000; ++i) {
		Vector3 randPos   = Vector3(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 9 + 1), -static_cast<float>(rand() % 1000));
		Vector3 randColor = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand()))/ RAND_MAX;
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		lightList->addLight(
			PointLight(
				randPos,
				randColor,
			10.0f));
	}
   
	testFloorMain = std::make_unique<Skyscraper>();
	testFloorMain.get()->testDraw("test");
   /*//Road Network Turtlewalker
	testNetwork.get()->generateInitialSegments("FFFFFFFFFFFFFFF-FF-FF-FFH+F+F+FF+FF+FF+FFFFFFFFF+FF-F-FF-FFF-FFF");
	testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");
	testNetwork.get()->setAngle(45);
	for (int i = 0; i < 5; i++) {
		testNetwork.get()->generateAdditionalSegments("FFFF-FF+F+F+F", ((i * 3) + 1) + 2, false);
		testNetwork.get()->generateAdditionalSegments("H-F+FFF+F+H+F", ((i * i) + 1) + 2, true);
	}
	testNetwork.get()->cleanRoadNetwork();
	testNetwork.get()->saveTestNetwork("test-network");*/

	//}
	lightList->setSun(Sun(Vector3(0.5f, -1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f)));

	graphics.setLightList(lightList.get());

	physics = std::make_unique<Physics>();
	player->init(physics.get());
	
	map = std::make_unique<Map>(graphics, config, physics.get());
   map->setDistrictColorCoding( isDebugging );
	initAI();
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

   auto playerVehicle = player->getVehicle();
	playerVehicle->setPosition(Vector3(15,0,-15));

	testObjective = new GameObject();
	testObjective->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(testObjective);
	testObjective->setColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	testObjective->setPosition(Vector3(9.0f, 0.0f, 9.0f));


	playerLight = lightList->addLight(SpotLight(playerVehicle->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 10.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

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
		Vector4(0.3,0.3,0.3,1),
		Vector4(0.2,0.2,0.2,1)
	};
	graphics.setParticleColorNSize(colorsP, 4, size1, size2);
	graphics.setParticle2ColorNSize(colorP2, 2, size1+0.1f, size2+0.1f);

	Input::SetKeyboardPlayerID(0);
	//Bullet
	//buildingTest = std::make_unique<GameObject>();
	//buildingTest->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(buildingTest.get());
	//btRigidBody* tempo2 = physics->addBox(btVector3(-15, 0.0f, -15.0f), btVector3(10.0f, 100.0f, 10.0f), 0.0f);
	//buildingTest->setPosition(Vector3(-15, 0.0f, -15.0f));
	//buildingTest->setScale(Vector3(10.0f, 100.0f, 10.0f));
	//buildingTest->setColor(Vector4(0.5, 0.5, 0.5, 1));
	//buildingTest->setRigidBody(tempo2, physics.get());
}

PlayingGameState::~PlayingGameState()
{
	delete aStar;
	delete actorManager;
	delete testObjective;
}

void  PlayingGameState::ImGui_Driving()
{
	ImGui::Begin("OilSpillage");
	ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Time Left: %f", time);
	ImGui::Text(("Rotation: " + to_string(player->getRotator())).c_str());
	ImGui::Text("Driving Mode:");
	static int radioButtonValue = 1;
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
	Vector2 lDir = Input::GetDirectionL(0);
	Vector2 rDir = Input::GetDirectionR(0);
	float lStr = Input::GetStrengthL(0);
	float rStr = Input::GetStrengthR(0);
	bool status[4] = { Input::CheckButton(CONFIRM, UP, 0), Input::CheckButton(CONFIRM, HELD, 0), Input::CheckButton(CONFIRM, RELEASED, 0), Input::CheckButton(CONFIRM, PRESSED, 0) };
	ImGui::Text(("Cam Pos: " + std::to_string(camPos.x) + " " + std::to_string(camPos.y) + " " + std::to_string(camPos.z)).c_str());
	ImGui::Text(("Cam Rot: " + std::to_string(camRot.x) + " " + std::to_string(camRot.y) + " " + std::to_string(camRot.z)).c_str());
	ImGui::Text(("\n-- PLAYER 0 --\nConfirm Status - Up: " + std::to_string(status[0]) + " Held: " + std::to_string(status[1]) + " Released: " + std::to_string(status[2]) + " Pressed: " + std::to_string(status[3])).c_str());
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
	ImGui::SetWindowSize({ 100,150 });

	ImGui::Text(("Groups: " + std::to_string(actorManager->groups.size())).c_str());
	for (int i = 0; i < actorManager->groups.size(); i++)
	{
		ImGui::Text(("Group " + std::to_string(i) + ":" + std::to_string(actorManager->groups.at(i).size())).c_str());
	}
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
	if ( ImGui::Button("Re-generate") ) {
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

void  PlayingGameState::update(float deltaTime)
{
	/*-------------------------UPDATING-------------------------*/
	if (currentMenu == PlayingGameState::MENU_PLAYING)
	{
		if (Input::IsKeyDown_DEBUG(Keyboard::E)) {
			deltaTime /= 4;
		}

#ifdef _DEBUG
		if (Input::CheckButton(ACTION_1, PRESSED, 0))
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
		else
		{
			Game::setState(Game::STATE_MENU);
		}
#endif // !_DEBUG

		//Bullet
		//player->getVehicle()->getRigidBody()->(btVector3(Input::GetDirectionL(0).x * deltaTime * 1000, 0, Input::GetDirectionL(0).y * deltaTime * 1000), btVector3(0, 0, 0));

		//player->getVehicle()->setPosition(Vector3(player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getX(), player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getY(), player->getVehicle()->getRigidBody()->getWorldTransform().getOrigin().getZ()));
		//player->getVehicle()->updateRigidBody();
		
		auto playerVehicle { player->getVehicle() };
		
		player->update(       deltaTime );
		physics->update(      deltaTime );
		actorManager->update( deltaTime, playerVehicle->getPosition() );
		camera->update(       deltaTime );

		btVector3 positionCam { playerVehicle->getRigidBody()->getWorldTransform().getOrigin() };

		camera->setPosition( Vector3( positionCam.getX(),
		                              positionCam.getY(),
		                              positionCam.getZ() ) + Vector3(.0f, cameraDistance, .0f) );

		Vector3 spotlightDir { sin(playerVehicle->getRotation().y),
		                       .0f,
		                       cos(playerVehicle->getRotation().y) };

      playerLight->setDirection( spotlightDir );

		Vector3 spotlightPos { playerVehicle->getPosition().x,
		                       playerVehicle->getPosition().y + 1,
		                       playerVehicle->getPosition().z };

		spotlightPos += spotlightDir * 1;

		playerLight->setPos( spotlightPos );
		
		timerForParticle += deltaTime;
		if ( timerForParticle > .01f )
		{
			/*graphics.addParticle( player->getVehicle()->getPosition() + Vector3(0, 5, 0),
			                      5 * Vector3(Input::GetDirectionR(0).x,
			                      0,
			                      Input::GetDirectionR(0).y),
			                      addNrOfParticles, lifeTime, randomPosPower);*/
			timerForParticle = 0;
		}
		
		if ( player->isDead() )
		{
			changeTime( -30.0f );
			player->resetHealth();
		}
	}
	
	/*-------------------------RENDERING-------------------------*/
	// render all objects
	graphics.setSpotLighShadow(playerLight);
	graphics.render( camera.get(), deltaTime );
	
	// render UI
	menues[MENU_PLAYING]->update( deltaTime );
	if ( currentMenu != MENU_PLAYING )
		menues[currentMenu]->update( deltaTime );
	else if ( Input::CheckButton(MENU, PRESSED, 0) )
		setCurrentMenu( PlayingGameState::MENU_PAUSED );
	
	//Render all objects
	
	//testNetwork.get()->drawRoadNetwork(&graphics);
	
	if constexpr ( isDebugging ) {
	   ImGui_ImplDX11_NewFrame();
	   ImGui_ImplWin32_NewFrame();
	   ImGui::NewFrame();
	   ImGui_Driving();
	   ImGui_ProcGen();
	   ImGui_AI();
	   ImGui_Particles();
	   ImGui_Camera();
	   ImGui::Render();
	   ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
	
	graphics.presentScene();
}

F32 const &PlayingGameState::getTimeRef() const noexcept {
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