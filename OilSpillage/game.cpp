#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "PG/Map.hpp"
#include "PG/Walker.hpp"

Graphics Game::graphics = Graphics();

void Game::addQuad(int x)
{
	GameObject* object2 = new GameObject;
	object2->mesh = graphics.getMeshPointer("Cube");
	object2->setColor(Vector4(0, 1, 0, 1));
	graphics.addToDraw(object2);
	object2->setPosition(Vector3(static_cast<float>(x), 0.0f, 0.0f));
	object2->setTexture(graphics.getTexturePointer("brickwall.tga"));

}
void Game::generateMap() {
	// TODO: init models
	static Map map(40, 40);
#define NO_TERMINAL_COLORS

	std::ofstream f1("road_gen_debug_output_pregen.txt");
	if (f1.is_open()) {
		f1 << map;
		f1.close();
	}

	Walker generator {
		map,    // the map to work on (mutate)
		4,      // depth
		80,    // min length
	    120,    // max length
		0.5f,   // child length factor
		5.0f,  // turn probability
		2.f,   // child turn probability factor
		12.0f,   // branch probability
		0.75f,  // child branch probability factor
		42069     // seed
	};

	generator.generate();

	std::ofstream f2("road_gen_debug_output.txt");
	if (f2.is_open()) {
		f2 << map;
		f2.close();
	}

	tiles = map.load_as_models(graphics); // TODO: RAII!
	for (auto& tile : tiles) {
		graphics.addToDraw(&tile);
		tile.setColor({ 0.6f, 0.6f, 0.6f, 1.0f });
		//tile.setTexture(graphics.getTexturePointer("brickwall.tga"));
		tile.setScale(Vector3{ 0.01f,  0.005f, 0.01f });
	}
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


	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow( window->handle );
	graphics.loadMesh("sda");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall.tga");
	graphics.loadModel("Dummy_Roller_Melee");

	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("Cube");
	//graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(7770.0f, 0.0f, 0.0f)); // moved away to "disable"
	testObject->setScale(Vector3(0.2f, 0.2f, 0.2f));
	//testObject->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject->setTexture(graphics.getTexturePointer("brickwall.tga"));
	//testObject->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(7777.0f, 0.0f, 0.0f)); // moved away to "disable"
	testObject2->setScale(Vector3(0.01f, 0.01f, 0.01f));
	//testObject2->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject2->setTexture(graphics.getTexturePointer("brickwall.tga"));
	
	this->testObject3 = new GameObject;
	testObject3->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject3);
	testObject3->setPosition(Vector3(7770.0f, -1.0f, 0.0f)); // moved away to "disable"
	testObject3->setScale(Vector3(50.0, 1.0, 50.0));
	testObject3->setTexture(graphics.getTexturePointer("brickwall.tga"));
	testObject3->setColor(Vector4(2.0, 2.0, 2.0, 1.0));

	aiObject = new AIPlayer();
	aiObject->mesh = graphics.getMeshPointer("Cube");
	aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	//aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	graphics.addToDraw(aiObject);
	//AiTestObject = new GameObject;
	//AiTestObject->mesh = graphics.getMeshPointer("Cube");
	//AiTestObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	//AiTestObject->setColor(Vector4(1.0f, 0.0f,0.0f,1.0f));
	//graphics.addToDraw(AiTestObject);

	parentTest = new GameObject;
	parentTest->mesh = graphics.getMeshPointer("Cube");
	parentTest->setPosition(Vector3(-3.0f, 0.0f, 0.0f));
	parentTest->setScale(Vector3(0.5f, 0.5f, 0.5f));
	parentTest->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	graphics.addToDraw(parentTest);

	childTest = new GameObject;
	childTest->mesh = graphics.getMeshPointer("Cube");
	childTest->setPosition(Vector3(-3.0f, 0.0f, 0.0f));
	childTest->setColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	childTest->parent = parentTest;
	graphics.addToDraw(childTest);

	// TODO reafctor out
	graphics.loadModel("Road_pavement");
	graphics.loadModel("Road_deadend");
	graphics.loadModel("Road_bend");
	graphics.loadModel("Road_straight");
	graphics.loadModel("Road_3way");
	graphics.loadModel("Road_4way");
	generateMap();

#if _DEBUG
	// light tests
	lightList.addLight(SpotLight(Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 50.f, Vector3(-2.f, -1.0f, 0.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f), 50.f, Vector3(2.f, -1.0f, 0.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f), 50.f, Vector3(0.f, -1.0f, 2.0f), 0.5));
	lightList.addLight(SpotLight(Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 50.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	

	lightList.removeLight(lightList.addLight(PointLight(Vector3(0, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 5000.f)));
	
	for (int i = 0; i < 50; ++i)
	{
		Vector3 randPos = Vector3(rand() % 101 - 50, 0.01, rand() % 101 - 50);
		Vector3 randColor = Vector3(rand(), rand(), rand()) / RAND_MAX;
		randColor.Clamp(Vector3(0.2, 0.2, 0.2), Vector3(1.0, 1.0, 1.0));

		lightList.addLight(
			PointLight(
				randPos,
				randColor,
				15.0f));
	}
#endif
	lightList.setSun(Sun(Vector3(0.0, -1.0, 1.0), Vector3(1.0, 0.8, 0.6)));
	graphics.setLightList(&lightList);

	player.init();

	playerLight = lightList.addLight(SpotLight(player.getVehicle()->getPosition(), Vector3(0.8f, 0.8f, 0.8f), 50.f, Vector3(0.f, -1.0f, -2.0f), 0.5));

	RadioButtonValue = 0;
}

template <typename T>
void delete_pointed_to(T* const ptr)
{
	delete ptr;
}

void Game::run()
{
	//Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)& countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	//Initial previous time	
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
		Input::Update();
		Sound::Update(deltaTime);
		//Game logic
		//Graphics

		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;
		

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

		if (Input::IsKeyDown_DEBUG(Keyboard::A))
			this->testObject->addRotation(Vector3(0.00f, 0.01f * deltaTime * 200, 0.00f));
		if(Input::IsKeyDown_DEBUG(Keyboard::D))
			this->testObject->addRotation(Vector3(0.00f, -0.01f * deltaTime * 200, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::W))
			this->testObject->addRotation(Vector3(0.01f * deltaTime * 200, 0.00f, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::S))
			this->testObject->addRotation(Vector3(-0.01f * deltaTime * 200, 0.00f, 0.00f));
		
		this->player.update(deltaTime);
		Vector3 spotlightDir = Vector3((sin(player.getVehicle()->getRotation().y)), 0, (cos(player.getVehicle()->getRotation().y)));
		Vector3 spotlightPos = Vector3(player.getVehicle()->getPosition().x, player.getVehicle()->getPosition().y + 1, player.getVehicle()->getPosition().z);
		spotlightPos += spotlightDir * 1;
		playerLight->setDirection(spotlightDir);
		playerLight->setPos(spotlightPos);

		this->aiObject->Update(deltaTime);
		this->camera.update(deltaTime);
		this->camera.setPosition(this->player.getVehicle()->getPosition() + Vector3(0, 15, 0));
		
		this->parentTest->setRotation(Vector3(0.0f, cos(curTime * secPerCount) * 2.0f, 0.0f));

		this->graphics.render(&this->camera);
		this->graphics.getdebugger()->DrawCube(this->testObject2->getTheAABB().maxPos, this->testObject2->getTheAABB().minPos, this->testObject2->getPosition(), Vector3(0, 1, 0));
		
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
	delete this->testObject;
	delete this->testObject2;
	delete this->testObject3;
	delete this->aiObject;
	delete this->parentTest;
	delete this->childTest;
}
