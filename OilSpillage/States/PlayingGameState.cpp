#include "PlayingGameState.h"
#include "../Sound.h"
#include "../Input.h"
#include "../UI/Playing/UIPlaying.h"
#include "../UI/Playing/UIPaused.h"
#include "../UI/Menu/UIOptions.h"
#include "../PG/MinimapTextureGenerator.hpp"
#include "../PG/Profiler.hpp"
#include <future>

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
	//testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");

	if (true)
		testNetwork.get()->setAngle(electricNew);


	testNetwork.get()->generateAdditionalSegments("F-F", 1, false);
	testNetwork.get()->setAngle(40);

	testNetwork.get()->generateAdditionalSegments("F-F+F-F", 2, false);
	testNetwork.get()->generateAdditionalSegments("F-F-F+F", 3, true);
	
}

void PlayingGameState::initAI()
{
	aStar = new AStar(map->getTileMap());
	actorManager = new ActorManager(aStar, physics.get());
	aStar->generateTileData(map->getTileMap());
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//	actorManager->createSpitFire(map->getStartPositionInWorldSpace().x + i, map->getStartPositionInWorldSpace().z + j);
				//actorManager->createTurret(map->getStartPositionInWorldSpace().x + i + 50, map->getStartPositionInWorldSpace().z + j + 50, 1);
			//	actorManager->createAttacker(map->getStartPositionInWorldSpace().x + i + 50, map->getStartPositionInWorldSpace().z + j + 50, 3);
			//actorManager->createSwarm(map->getStartPositionInWorldSpace().x + i + 50, map->getStartPositionInWorldSpace().z + j + 50);
		}
	}
}

PlayingGameState::PlayingGameState() : graphics(Game::getGraphics()), time(360.0f), currentMenu(MENU_PLAYING)
{

#if defined(_DEBUG) || defined(RELEASE_DEBUG)
	pausedTime = false;
#endif // _DEBUG
	this->current_item = NULL;

	rng.seed(config.seed); // gör i konstruktorn
	lightList = std::make_unique<LightList>();
	camera = std::make_unique<DynamicCamera>();
	testNetwork = std::make_unique<RoadNetwork>(1351, Vector2(32.0f, 32.0f), Vector2(-32.0f,-32.0f), 0); //Int seed, max pos, min pos, angle in degrees
	graphics.createFrustumBuffer(camera.get());


	graphics.loadMesh("Cube");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall");
	graphics.loadTexture("grass3");

	graphics.loadTexture("brickwallnormal");
	graphics.loadModel("Roller_Melee");
	graphics.loadModel("Entities/Turret");
	graphics.loadModel("Entities/Player", Vector3(3.14f / 2, 0, 0));

	//graphics.loadModel("Roads/Metro/0000");
	//graphics.loadModel("Roads/Metro/0001");
	//graphics.loadModel("Roads/Metro/0010");
	//graphics.loadModel("Roads/Metro/0011");
	//graphics.loadModel("Roads/Metro/0100");
	//graphics.loadModel("Roads/Metro/0101");
	//graphics.loadModel("Roads/Metro/0110");
	//graphics.loadModel("Roads/Metro/0111");
	//graphics.loadModel("Roads/Metro/1000");
	//graphics.loadModel("Roads/Metro/1001");
	//graphics.loadModel("Roads/Metro/1010");
	//graphics.loadModel("Roads/Metro/1011");
	//graphics.loadModel("Roads/Metro/1100");
	//graphics.loadModel("Roads/Metro/1101");
	//graphics.loadModel("Roads/Metro/1110");
	//graphics.loadModel("Roads/Metro/1111");

	graphics.loadModel("Tiles/Quad_SS"); // single-sided
	graphics.loadTexture("Tiles/asphalt");
	graphics.loadTexture("Tiles/asphalt_nor");
	graphics.loadTexture("Tiles/grass");
	graphics.loadTexture("Tiles/grass_nor");
	// graphics.loadTexture("Tiles/grass_spec");
	graphics.loadTexture("Tiles/snow");
	graphics.loadTexture("Tiles/snow_nor");
	// graphics.loadTexture("Tiles/snow_spec");
	graphics.loadTexture("Tiles/desert");
	graphics.loadTexture("Tiles/desert_nor");
	// graphics.loadTexture("Tiles/desert_spec");
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
	graphics.loadModel("Houses/testHouse6");
	graphics.loadModel("Houses/houseMaterial");
	graphics.loadModel("Houses/houseMaterial2");
	graphics.loadModel("Houses/houseMaterial3");


	player = std::make_unique<Vehicle>();
	player->makePlayer();
	//if constexpr ( isDebugging ) {
		// light tests
	lightList->addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 1.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	lightList->addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 1.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	lightList->addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 1.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	lightList->addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 1.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	lightList->removeLight(lightList->addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));

	for (int i = 0; i < 100; ++i) {
		Vector3 randPos = Vector3(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 9 + 1), -static_cast<float>(rand() % 1000));
		Vector3 randColor = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())) / RAND_MAX;
		randColor.Clamp(Vector3(0.2f, 0.2f, 0.2f), Vector3(1.0f, 1.0f, 1.0f));

		lightList->addLight(
			PointLight(
				randPos,
				randColor,
				10.0f));
	}

	
	 //Road Network Turtlewalker
	//FFFFFFFFFFFFFFF - FF - FF - FFH + F + F + FF + FF + FF + FFFFFFFFF + FF - F - FF - FFF - FFF


	testNetwork.get()->generateInitialSegments("F");
	//testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");

	testNetwork.get()->setAngle(30);
	testNetwork.get()->generateAdditionalSegments("F--FF++FF", 1, false);
	/*testNetwork.get()->generateAdditionalSegments("F-F", 1, false);
	for (int i = 2; i < 4; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-F-F-F", i, i%2);
	}
	for (int i = 0; i < 5; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-FF-F+F-FF-F-F-F", i + 4, i % 2);
		testNetwork.get()->generateAdditionalSegments("F-F-F-FF-F+F-F-F", i + 4*2, i % 2);
	}
	for (int i = 0; i < 5; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-FF-F+F-FF-F-F-F", i + 4, i % 2);
		testNetwork.get()->generateAdditionalSegments("F-F-F-FF-F+F-F-F", i + 4 * 2, i % 2);
	}*/
	//for (int i = 0; i < 5; i++) 
	//{
	//	// testNetwork.get()->generateAdditionalSegments("H-F+FFF+F+H+F", ((i * i) + 1) + 2, true);
	//}
	//testNetwork.get()->setAngle(90+45);

	//for (int i = 0; i < 5; i++)
	//{
	//	testNetwork.get()->generateAdditionalSegments("F-F+F+FF-F-F+F", i, false);
	//	testNetwork.get()->generateAdditionalSegments("H-F+FFF+F+H+F", ((i * i) + 1) + 2, true);
	//}
	/* testNetwork.get()->cleanRoadNetwork();
	 testNetwork.get()->saveTestNetwork("test-network");
	*/
	//}

	lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));

	graphics.setLightList(lightList.get());
	SpotLight tempLight(Vector3(0, 0, 0), Vector3(0.9, 0.5, 0), 1.0f, Vector3(0, 0, 0), 0.0f);
	this->player->setSpotLight(lightList->addLight(tempLight));

	LaserLight tempLaserLight(Vector3(0.0, 0.0, 0.0), Vector3(0.9, 0.1, 0.9), 10.0, Vector3(1.0, 0.0, 0.0), 10.0);
	this->player->setLaserLight(lightList->addLight(tempLaserLight));

	physics = std::make_unique<Physics>();
	player->init(physics.get());

	map = std::make_unique<Map>(graphics, config, physics.get());
	map->setDistrictColorCoding(isDebugging);
	// Minimap stuff
	auto tilemap = map->getTileMap();
	topLeft = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileScaleFactor.x, 0, config.tileScaleFactor.z);
	bottomRight = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileScaleFactor.x, 0, -config.tileScaleFactor.z);
	// Needs to be loaded before the menues
	minimap = createMinimapTexture(*map);

	menues[MENU_PLAYING] = std::make_unique<UIPlaying>();
	menues[MENU_PLAYING]->init();
	menues[MENU_PAUSED] = std::make_unique<UIPaused>();
	menues[MENU_PAUSED]->init();
	menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	menues[MENU_OPTIONS]->init();

	Vector3 startPos = map->getStartPositionInWorldSpace();
	auto playerVehicle = player->getVehicle();
	playerVehicle->setPosition(startPos + Vector3(.0f, 0.00f - 1.2f, .0f));
	player->getVehicleBody1()->setPosition(startPos + Vector3(.0f, 0.65f - 1.2f, .0f));

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
	camera->startCinematic(&points, true);
	Vector4 colorP2[] = {
		Vector4(0.03f,0.03f,0.03f,1),
		Vector4(0.9f, 0.9f, 0.05f, 1)
	};

	graphics.setParticle2ColorNSize(colorP2, 2, 0.025f, 0.05f);


	addPowerUp(PowerUp(Vector3(10, 0.0, -500), PowerUpType::Star, 30.0));

	//objectives.addObjective(TypeOfMission::KillingSpree, 120, 20, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 5, "Pick up the important", TypeOfTarget::Crate);
	objectives.addObjective(TypeOfMission::GetToPoint, 0, 5, "Go to the exit now!!!", TypeOfTarget::Crate);

	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::KillingSpree, 240, 75, "Kill the enemies");
	objectives.addObjective(TypeOfMission::KillingSpree, 120, 50, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 6, "Pick up the important");
	objectives.addObjective(TypeOfMission::KillingSpree, 120, 25, "Kill the enemies");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 2, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 8, "Pick up the important");
	objectives.addObjective(TypeOfMission::FindAndCollect, 240, 7, "Pick up the important");

	
	this->graphics.setTestParticleSystem(this->graphics.getParticleSystem("explosion"));
	this->fillTestParticle();



	//Bullet
	/*buildingTest = std::make_unique<GameObject>();
	graphics.loadModel("Vehicles/Player");
	buildingTest->mesh = graphics.getMeshPointer("Vehicles/Player");
	graphics.addToDraw(buildingTest.get());
	btRigidBody* tempo2 = physics->addBox(btVector3(-15, 0.0f, -15.0f), btVector3(10.0f, 100.0f, 10.0f), 0.0f);
	buildingTest->setPosition(Vector3(-15, 0.0f, -15.0f));
	buildingTest->setScale(Vector3(10.0f, 100.0f, 10.0f));
	buildingTest->setColor(Vector4(0.5, 0.5, 0.5, 1));
	buildingTest->setRigidBody(tempo2, physics.get());*/

#ifndef _DEBUG
	spawnObjects();
#endif
	count = 0;
	prevAccelForce = Vector3(0, 0, 0);
	accelForce = Vector3(0, 0, 0);
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
	ImGui::Text(("Rotation: " + std::to_string(player->getRotator())).c_str());
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
	ImGui::Text(("nr of enemies: " + std::to_string(nrOfEnemies)).c_str());
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
	/*float goalX = actorManager->groups.at(0).actors.at(0)->destination.x;
	float goalZ = actorManager->groups.at(0).actors.at(0)->destination.z;
	ImGui::Text((("GoalPosX: " + std::to_string(goalX)).c_str()));
	ImGui::Text((("GoalPosZ: " + std::to_string(goalZ)).c_str()));

	float posX = actorManager->groups.at(0).actors.at(0)->getPosition().x;
	float posZ = actorManager->groups.at(0).actors.at(0)->getPosition().z;

	ImGui::Text((("PosX: " + std::to_string(posX)).c_str()));
	ImGui::Text((("PosZ: " + std::to_string(posZ)).c_str()));

	float distance = (actorManager->groups.at(0).actors.at(0)->destination - actorManager->groups.at(0).actors.at(0)->getPosition()).Length();

	ImGui::Text((("Distance: " + std::to_string(distance)).c_str()));

	float pathSize = actorManager->groups.at(0).actors.at(0)->path->size();
	ImGui::Text((("Path size: " + std::to_string(pathSize)).c_str()));*/
	ImGui::Text(("VelocityX: " + std::to_string(this->player->getVehicle()->getRigidBody()->getLinearVelocity().getX())).c_str());
	ImGui::Text(("VelocityY: " + std::to_string(this->player->getVehicle()->getRigidBody()->getLinearVelocity().getY())).c_str());
	ImGui::Text(("VelocityZ: " + std::to_string(this->player->getVehicle()->getRigidBody()->getLinearVelocity().getZ())).c_str());
	/*	+ std::to_string(player->getVehicle()->getPosition().y).c_str()
							+ std::to_string(player->getVehicle()->getPosition().z).c_str()));*/
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
	static Bool shouldColorCodeDistricts{ true };
	ImGui::Begin("Map Generation:");
	if (static bool isFirstFrame = true;  isFirstFrame) {
		ImGui::SetWindowPos({ 0,  75 });
		ImGui::SetWindowSize({ 525, 475 });
		isFirstFrame = false;
	}

	ImGui::Separator();

	// debug colors toggle:
	bool shouldColorCodeDistrictsPrevFrame = shouldColorCodeDistricts;
	ImGui::Checkbox("Show district colors", &shouldColorCodeDistricts);
	if (shouldColorCodeDistricts != shouldColorCodeDistrictsPrevFrame)
		map->setDistrictColorCoding(shouldColorCodeDistricts);

	ImGui::Separator();

	// map:
	ImGui::InputInt2("TileMap dimensions", &config.dimensions.data[0]);
	if (config.dimensions.x < 1)
		config.dimensions.x = 1;
	if (config.dimensions.y < 1)
		config.dimensions.y = 1;

	// (TODO! bugged!) ImGui::InputFloat3( "Tile scale", &config.tileScaleFactor.data[0] );

	ImGui::InputFloat("Floor height factor", &config.buildingFloorHeightFactor);

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
		player->getVehicle()->getRigidBody()->setLinearFactor(btVector3(.0, .0f, .0f));

		map = nullptr; // clear, then regenerate:
		map = std::make_unique<Map>(graphics, config, physics.get());

		player->getVehicle()->setPosition(map->getStartPositionInWorldSpace());
		player->getVehicleBody1()->setPosition(map->getStartPositionInWorldSpace() + Vector3(.0f, .55f, .0f));
		map->setDistrictColorCoding(shouldColorCodeDistricts);
		minimap = createMinimapTexture(*map);
		aStar->generateTileData(map->getTileMap());
		// minimap stuff
		auto tilemap = map->getTileMap();
		topLeft = tilemap.convertTilePositionToWorldPosition(0, 0) + Vector3(-config.tileScaleFactor.x, 0, config.tileScaleFactor.z);
		bottomRight = tilemap.convertTilePositionToWorldPosition(config.dimensions.x - 1, config.dimensions.y - 1) + Vector3(config.tileScaleFactor.x, 0, -config.tileScaleFactor.z);

		graphics.reloadTexture(minimap);
		static_cast<UIPlaying*>(menues[MENU_PLAYING].get())->resetMinimapFog();
		player->getVehicle()->getRigidBody()->setLinearFactor(btVector3(1.0f, .0f, 1.0f));
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
	auto maybeHospitalTilePos = map->getNearestFoundHospitalTilePos(player->getVehicle()->getPosition(), *ui);
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
	
	/*testNetwork->clearSegments();
	testNetwork.get()->generateInitialSegments("F");
	testNetwork->setAngle(40.0f);
	for (int i = 2; i < 4; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-F-F-F", i, i % 2);
	}
	for (int i = 0; i < 5; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-FF-F+F-FF-F-F-F", i + 4, i % 2);
		testNetwork.get()->generateAdditionalSegments("F-F-F-FF-F+F-F-F", i + 4 * 2, i % 2);
	}
	for (int i = 0; i < 5; i++)
	{
		testNetwork.get()->generateAdditionalSegments("F-FF-F+F-FF-F-F-F", i + 4, i % 2);
		testNetwork.get()->generateAdditionalSegments("F-F-F-FF-F+F-F-F", i + 4 * 2, i % 2);
	}*/
	//this->createElectric(rand() % 360, deltaTime);
	/*-------------------------UPDATING-------------------------*/
	if (currentMenu == PlayingGameState::MENU_PLAYING)
	{
		if (Input::isKeyDown_DEBUG(Keyboard::E)) {
			deltaTime /= 4;
		}
		if (Input::isKeyDown_DEBUG(Keyboard::Q)) {
			if (player->getDrivingMode() == 0) {
				player->setDrivingMode(1);
			}
			else if (player->getDrivingMode() == 1) {
				player->setDrivingMode(0);
			}
		}

#if defined(_DEBUG) || defined(RELEASE_DEBUG)
		if (Input::isKeyDown_DEBUG(Keyboard::LeftAlt))
		{
			pausedTime = !pausedTime;
		}

		if (!pausedTime && time > 0.0f)
		{
			time = std::max(time - deltaTime, 0.0f);
		}
		else if (time <= 0.0f)
		{
			Game::setState(Game::STATE_MENU);
		}
#else
		if (time > 0.0f)
		{
			time = std::max(time - deltaTime, 0.0f);
		}
		else if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
		{
			Game::setState(Game::STATE_MENU);
		}
#endif // !_DEBUG

		auto playerVehicle{ player->getVehicle() };
		prevAccelForce = Vector3(playerVehicle->getRigidBody()->getLinearVelocity());
		player->updatePlayer(deltaTime);
		physics->update(deltaTime);

		Vector3 cameraMovement(player->getCameraDistance(deltaTime));
		btVector3 positionCam{ playerVehicle->getRigidBody()->getWorldTransform().getOrigin() };
		camera->setPosition(Vector3(positionCam.getX(),
			positionCam.getY() / 3,
			positionCam.getZ()) + Vector3(cameraMovement.x, cameraMovement.y + cameraDistance, cameraMovement.z));
		camera->update(deltaTime);

		Vector3 tempCamPos = camera->getPosition() * Vector3(1.0f, 0.0f, 1.0f) + Vector3(0.0f, positionCam.getY() / 3 + cameraMovement.y, 0.0f);
		Sound::updateListener(tempCamPos, tempCamPos + Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), prevAccelForce);
		
		size_t playerBulletCount;
		Bullet* playerBullets = player->getBulletArray(playerBulletCount);

		if (spawnTimer % 200 == 0)
		{
			//actorManager->spawnAttackers(generateObjectivePos(50.0f, 100.0f),1);
			//actorManager->spawnSwarm(generateObjectivePos(50.0f, 100.0f));
			nrOfEnemies += 12;
			//actorManager->spawnChaseCars(generateObjectivePos(50.0f, 100.0f));
			spawnTimer = 0;
		}
		spawnTimer++;
		//spawn Boss
		//if (spawnTimer % 10 == 0)
		//{
		//	actorManager->spawnBoss(Vector3(player->getVehicle()->getPosition().x + 5,
		//		player->getVehicle()->getPosition().y,
		//		player->getVehicle()->getPosition().z + 5), 1);
		//	spawnTimer = 0;
		//}
		//spawnTimer = 1;

		for (std::unique_ptr<PowerUp>& p : powerUps)
		{
			p->update(deltaTime);
			if (p->isActive())
			{
				if (p->getAABB().intersectXZ(player->getVehicle()->getAABB()))
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

		actorManager->update(deltaTime, playerVehicle->getPosition());
		auto bulletThread = std::async(std::launch::async, &ActorManager::intersectPlayerBullets, actorManager, playerBullets, playerBulletCount);
		accelForce = Vector3(player->getVehicle()->getRigidBody()->getLinearVelocity().getX(), player->getVehicle()->getRigidBody()->getLinearVelocity().getY(), player->getVehicle()->getRigidBody()->getLinearVelocity().getZ()) - Vector3(prevAccelForce.x, prevAccelForce.y, prevAccelForce.z);
		player->setAccelForce(accelForce, deltaTime);
		player->setWheelRotation();
		//actorManager->intersectPlayerBullets(playerBullets, playerBulletCount);
		camera->update(deltaTime);
		objectives.update(player->getVehicle()->getPosition());
		Bullet::updateSoundTimer(deltaTime);
		bulletThread.get();
		player->updateWeapon(deltaTime);
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

			
			/*testNetwork->clearSegments();
			testNetwork.get()->setAngle(rand() % 360 + 1);

			testNetwork.get()->generateInitialSegments("+F");

			testNetwork.get()->setAngle(rand()%30+1);

			testNetwork.get()->generateInitialSegments("H--H--H--H--H--H--H--H");
			int scale = 10;
			std::string temp = "F--FFFF++FFFF-FFF--FF";
			std::string addedTemp = temp;
			for (int i = 1; i < scale; i++)
			{
				if (i % 2)
				{
					addedTemp.replace("-", "+");
				}
				temp += addedTemp;
			}
			if (rand() % 2)
			{
				temp += "FF-F+F+FF";
			}
			std::string branch = "FF-FF+F";
			if (rand() % 2)
			{
				branch += "F+FF-FF";
			}
			testNetwork.get()->generateAdditionalSegments(temp.c_str(), 1, rand() % 2);
			testNetwork.get()->generateAdditionalSegments(branch.c_str(), scale*2, rand() % 2);
			testNetwork.get()->generateAdditionalSegments(branch.c_str(), 12, rand() % 2);
			testNetwork.get()->generateAdditionalSegments(branch.c_str(), 22, rand() % 2);

			timer = 0.0f;
			int iteration = rand() % this->testNetwork->getSegments().size() + 10;
			iteration = this->testNetwork->getSegments().size();
			for (int i = 1; i < iteration; i++)
			{
				this->graphics.addTestParticle(Vector3(player->getVehicleBody1()->getPosition()) +Vector3(0,0,5.0f)+ Vector3(this->testNetwork->getSegments().at(i).firstPoint.x, this->testNetwork->getSegments().at(i).firstPoint.y, this->testNetwork->getSegments().at(i).firstPoint.z), Vector4(0, 0, 0, 0.0f), this->addNrOfParticles, this->lifeTime, this->randomPosPower);
			}
			*/

			this->graphics.addTestParticle(Vector3(player->getVehicleBody1()->getPosition()+Vector3(0.0f,0.0f,5.0f)), Vector4(0, 0, 0, 0.0f), this->addNrOfParticles, this->lifeTime, this->randomPosPower);
			timer = 0.0f;
		}

		
#ifndef _DEBUG
		updateObjects();
		paperCollision(deltaTime);
#endif

		

		btVector3 spotlightDir{ 0,
								 0,
								 1 };
		playerVehicle->getRotationQuaternion();
		btQuaternion bt5 = btQuaternion(this->player->getVehicleBody1()->getRigidBody()->getWorldTransform().getRotation());
		btTransform transform(bt5, btVector3(0, 0, 0));
		spotlightDir = transform * spotlightDir;
		playerLight->setDirection(Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()));

		Vector3 spotlightPos{ playerVehicle->getPosition().x,
							   playerVehicle->getPosition().y + 1,
							   playerVehicle->getPosition().z };

		spotlightPos += Vector3(spotlightDir.getX(), spotlightDir.getY(), spotlightDir.getZ()) * 1;

		playerLight->setPos(spotlightPos);

		if (actorManager->distanceToPlayer(Vector3(positionCam)) < 25.0f || this->time <= 20.0f)
		{
			Sound::fadeSoundtrack(true, 1.0f);
		}
		else
		{
			Sound::fadeSoundtrack(false, 3.0f);
		}

		/*if ((actorManager->distanceToPlayer(Vector3(positionCam)) < 40.0f && soundAggro < 1.0f) || this->time <= 20.0f) {
			soundAggro += 0.2f * deltaTime;
		}
		else if (soundAggro > 0.0f) {
			soundAggro -= 0.1f * deltaTime;
		}
		Sound::changeVolume(L"data/sound/OilSpillageSoundtrack1_Aggressive.wav", soundAggro);*/

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

	graphics.clearScreen(Vector4(0, 0, 0, 1));
	graphics.setSpotLightShadow(playerLight);
	graphics.render(camera.get(), deltaTime);

	// render UI
	menues[MENU_PLAYING]->update( deltaTime );
	if ( currentMenu != MENU_PLAYING )
		menues[currentMenu]->update( deltaTime );
	else if ( Input::checkButton(Keys::MENU, States::PRESSED) )
		setCurrentMenu( PlayingGameState::MENU_PAUSED );
	

	//Render all objects

	//testNetwork.get()->drawRoadNetwork(&graphics);

//#if defined(_DEBUG) || defined(RELEASE_DEBUG) //Set RELEASE_DEBUG to false to deactivate imgui in release!
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui_Driving();
	//ImGui_ProcGen();
	//ImGui_AI();
	ImGui_Particles();
	ImGui_Camera();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//#endif // !_DEBUG

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
	graphics.loadModel("Entities/Barrel");
	graphics.loadModel("Entities/Garbage_Bag");
	graphics.loadModel("Entities/Quad");
	physicsObjID = 0;
	physicsObjects.reserve(300);
	btRigidBody* tempo2;
	float randomValue;
	for (int i = 0; i < 20; i++) {
		physicsObjects.emplace_back(std::make_unique<GameObject>());
		auto objPtr = physicsObjects.back().get();
		graphics.loadModel("Cube");
		objPtr->mesh = graphics.getMeshPointer("Cube");
		graphics.loadTexture("brownPaperCardboard");
		objPtr->setTexture(graphics.getTexturePointer("brownPaperCardboard"));
		graphics.addToDraw(objPtr);
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
		tempo2 = physics->addCylinder(btVector3(-200, 0.2f + i, -200), btVector3(size, size, size), 0.01f);
		objPtr->setPosition(Vector3(-200, 0.2f + i, -200));
		objPtr->setScale(Vector3(size, size, size));
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
		objPtr->mesh = graphics.getMeshPointer("Entities/Garbage_Bag");
		objPtr->setTexture(graphics.getMaterial("Entities/Garbage_Bag").diffuse);
		graphics.addToDraw(objPtr);
		objPtr->setPosition(Vector3(0, 0, 0));
		randomValue = (rand() % 3 + 8) * 0.125f;
		size = 0.30f * randomValue;
		objPtr->setScale(Vector3(size, size, size));
		tempo2 = physics->addSphere(size, btVector3(0, 0.0f, 0), 0.01f);
		objPtr->setPosition(Vector3(0, 0, 0));
		//objPtr->setColor(Vector4(0.8, 0.6, 0.38, 1));
		objPtr->setRigidBody(tempo2, physics.get());
		objPtr->getRigidBody()->setDamping(0, 0.98f);
		objPtr->getRigidBody()->setFriction(0.7f);
		objPtr->getRigidBody()->setActivationState(0);
		objPtr->getRigidBody()->setDeactivationTime(0.1f);
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

	//physicsObjects.emplace_back(std::make_unique<GameObject>());
	//auto objPtr = physicsObjects.back().get();
	//graphics.loadModel("Cube");
	//objPtr->mesh = graphics.getMeshPointer("Cube");
	//graphics.addToDraw(objPtr);
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
	//				graphics.loadModel("Cube");
	//				objPtr->mesh = graphics.getMeshPointer("Cube");
	//				graphics.addToDraw(objPtr);
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
		object->getPosition().z >(player->getVehicle()->getPosition().z + 35) ||
		object->getPosition().z < (player->getVehicle()->getPosition().z - 35)) {
		if (object->getRigidBody() != nullptr) {
			object->getRigidBody()->setActivationState(0);
		}
		int randomValue = rand() % 2 + 1;
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
		}
		else if (randomValue == 2) {
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
		float distance = (position - player->getVehicle()->getPosition()).Length();
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

void PlayingGameState::paperCollision(float deltaTime)
{

	float randomValue = 0;
	for (auto& obj : physicsObjects) {
		if (obj->getRigidBody() == nullptr) {
			randomValue = rand() % 2;
			if (((player->getVehicle()->getPosition() - obj->getPosition()).Length()) < 1.5f && abs(player->getVelocitySpeed()) > 8.0f && randomValue == 1) {
				randomValue = rand() % 20;
				randomValue = std::max(randomValue - 5.0f, 0.0f);
				obj->setVelocity(Vector3(player->getVehicle()->getRigidBody()->getLinearVelocity().getX(), randomValue * 0.5f, player->getVehicle()->getRigidBody()->getLinearVelocity().getZ()));
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

Vector3 PlayingGameState::getCameraPos()
{
	return this->camera->getPosition();
}
