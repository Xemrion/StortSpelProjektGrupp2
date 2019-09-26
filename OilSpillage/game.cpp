#include "game.h"
#include "Input.h"
#include "Sound.h"
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
	graphics.init(window, 90,this->camera);
	Sound::Init();
	
	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow(window->handle);
	graphics.loadMesh("sda");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall.tga");
	graphics.loadModel("Dummy_Roller_Melee");
	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("Cube");
	//graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	testObject->setScale(Vector3(0.2f, 0.2f, 0.2f));
	//testObject->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject->setTexture(graphics.getTexturePointer("brickwall.tga"));
	//testObject->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Dummy_Roller_Melee.bin");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(7.0f, 0.0f, 0.0f));
	testObject2->setScale(Vector3(0.01f, 0.01f, 0.01f));
	//testObject2->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject2->setTexture(graphics.getTexturePointer("brickwall.tga"));

	aiObject = new AIPlayer();
	aiObject->mesh = graphics.getMeshPointer("Cube");
	aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	graphics.addToDraw(aiObject);
	//AiTestObject = new GameObject;
	//AiTestObject->mesh = graphics.getMeshPointer("Cube");
	//AiTestObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	//AiTestObject->setColor(Vector4(1.0f, 0.0f,0.0f,1.0f));
	//graphics.addToDraw(AiTestObject);

	//Road network F: Forward, -: Turn Left, +: Turn Right, H: Half Forward
	this->testNetwork = new RoadNetwork(6243, Vector2(2, 2), Vector2(-2, -2), 15);
	this->testNetwork->generateInitialSegments("FFFFFFH-FFFF-FFFFF+FFFFF-FFF+F-F-F-F-F-H-H-HF-FF-F-FF-FF-FF-F-FFF-FFF-FFH-FF++F+FF+F");
	this->testNetwork->setAngle(90);
	this->testNetwork->generateAdditionalSegments("FFH-FFF+FFFH+FH", 4, true);
	this->testNetwork->generateAdditionalSegments("HF+FFF-FFFFF+FH", 15, false);
	this->testNetwork->setAngle(25);
	this->testNetwork->generateAdditionalSegments("FFF+F-F-F-F-F-F-F-F-F-FFH+F+F+F+F+H+F+F+H++H++H++H++H++H++H++H", 20, false);
	this->testNetwork->saveTestNetwork("test-net.txt");
	this->testNetwork->cleanRoadNetwork();
	this->testNetwork->saveTestNetwork("test-net-cleaned.txt");
	
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f),  50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f),  50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 50.f));

	graphics.setSunVector(Vector3(0.55f, 1.0f, 0.725f));

	player.init();
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

	this->aiObject->SetTarget(&this->player.getVehicle()->getPosition());
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
		if (Input::IsKeyDown_DEBUG(Keyboard::Escape))
		{
			Sound::PlaySoundEffect(L"test.wav");
			//Exit game
		}
		if (Input::IsKeyDown_DEBUG(Keyboard::E))
			deltaTime /= 4;


		if (Input::IsKeyDown_DEBUG(Keyboard::A))
			this->testObject->addRotation(Vector3(0.00f, 0.01f * deltaTime * 200, 0.00f));
		if(Input::IsKeyDown_DEBUG(Keyboard::D))
			this->testObject->addRotation(Vector3(0.00f, -0.01f * deltaTime * 200, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::W))
			this->testObject->addRotation(Vector3(0.01f * deltaTime * 200, 0.00f, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::S))
			this->testObject->addRotation(Vector3(-0.01f * deltaTime * 200, 0.00f, 0.00f));
		
		player.update(deltaTime);
		this->camera.setPos(this->player.getVehicle()->getPosition() + Vector3(0, 5, 0));
		this->graphics.render(this->camera);
		//this->graphics.getdebugger()->DrawCube(this->testObject2->getTheAABB().maxPos, this->testObject2->getTheAABB().minPos,this->testObject2->getPosition(), Vector3(0, 1, 0));
		std::string textUse;


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		//imgui button, slider etc
		ImGui::Begin("Gungame");
		ImGui::Text("Hold 'V' To move camera with mouse.");

		Vector2 lDir = Input::GetDirectionL(0);
		Vector2 rDir = Input::GetDirectionR(0);
		float lStr = Input::GetStrengthL(0);
		float rStr = Input::GetStrengthR(0);
		bool status[4] = { Input::CheckButton(CONFIRM, UP, 0), Input::CheckButton(CONFIRM, HELD, 0), Input::CheckButton(CONFIRM, RELEASED, 0), Input::CheckButton(CONFIRM, PRESSED, 0) };

		ImGui::Text(("\n-- PLAYER 0 --\nConfirm Status - Up: " + std::to_string(status[0]) + " Held: " + std::to_string(status[1]) + " Released: " + std::to_string(status[2]) + " Pressed: " + std::to_string(status[3])).c_str());
		ImGui::Text(("L Dir: " + std::to_string(lDir.x) + " " + std::to_string(lDir.y)).c_str());
		ImGui::Text(("L Str: " + std::to_string(lStr)).c_str());
		ImGui::Text(("R Dir: " + std::to_string(rDir.x) + " " + std::to_string(rDir.y)).c_str());
		ImGui::Text(("R Str: " + std::to_string(rStr)).c_str());
		ImGui::Text(("Accelerator: " + std::to_string(player.getAcceleratorX())).c_str());


		ImGui::End();

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		
		player.update(deltaTime);
		camera.setPos(player.getVehicle()->getPosition() + Vector3(0.0, 5.0, 0.0));
		
		graphics.setSunVector(Vector3(sin(curTime * secPerCount * 0.1f), cos(curTime * secPerCount * 0.1f), -0.5f));

		this->graphics.render(camera);

		this->testNetwork->drawRoadNetwork(&this->graphics);
		/*Vector3 tempPos = AiTestObject->getPosition();
		Vector4 tempColor = AiTestObject->getColor();
		this->AI.update(player.getVehicle()->getPosition(), deltaTime, tempPos, tempColor);
		AiTestObject->setPosition(tempPos);
		AiTestObject->setColor(tempColor);*/
		this->aiObject->Update(deltaTime);
		//deltaTime reset
		prevTime = curTime;
		
		this->graphics.presentScene();

	}
	delete this->testObject;
	delete this->testObject2;
	//delete this->AiTestObject;
	delete this->aiObject;
	delete this->testNetwork;
}
