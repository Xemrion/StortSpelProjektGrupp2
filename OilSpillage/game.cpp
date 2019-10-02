#include "game.h"
#include "Input.h"
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
	
	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow(window->handle);
	graphics.loadMesh("sda","fds");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadShape(SHAPE_QUAD);
	graphics.loadTexture("brickwall.tga", "brickwall.tga");
	graphics.loadModel("Dummy_Roller_Melee", "Dummy_Player_Car");
	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("Quad");
	graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(0.0f, -0.1f, 0.0f));
	testObject->setScale(Vector3(100.0f, 0.0f, 100.0f));
	testObject->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject->setTexture(graphics.getTexturePointer("brickwall.tga"));
	//testObject->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Dummy_Roller_Melee.bin");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(7.0f, 0.0f, 0.0f));
	testObject2->setScale(Vector3(0.01, 0.01, 0.01f));
	//testObject2->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject2->setTexture(graphics.getTexturePointer("brickwall.tga"));

	aiObject = new AIPlayer();
	aiObject->mesh = graphics.getMeshPointer("Cube");
	aiObject->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	aiObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	//graphics.addToDraw(aiObject);
	//AiTestObject = new GameObject;
	//AiTestObject->mesh = graphics.getMeshPointer("Cube");
	//AiTestObject->setPosition(Vector3(-7.0f, 0.0f, 5.0f));
	//AiTestObject->setColor(Vector4(1.0f, 0.0f,0.0f,1.0f));
	//graphics.addToDraw(AiTestObject);

	
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(-2.f, 1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(2.f, 1.0f, 0.0f), Vector3(0.3f, 0.3f, 1.0f),  50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(0.f, 1.0f, 2.0f), Vector3(1.0f, 0.3f, 0.3f),  50.f));
	graphics.addPointLight(PointLight(testObject2->getPosition() + Vector3(0.f, 1.0f, -2.0f), Vector3(0.3f, 1.0f, 0.3f), 50.f));

	graphics.setSunVector(Vector3(0.55f, 1.0f, 0.725f));

	player.init();
	Vector4 colorsP[4] =
	{
		Vector4(1.0f,1.0f,0.0f,0.5f),
		Vector4(1.0f,0.0f,0.0f,0.5f),
		Vector4(0.0f,0.0f,0.0f,0.5f),
		Vector4(0.1f,0.1f,0.1f,0.5f)
	};
	this->graphics.setParticleColorNSize(colorsP, 4, 0.05f, 0.10f);
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
		//Game logic
		//Graphics

		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;


		auto mouse = this->mouse->GetState();
		Vector2 mousePos = (1/camera.getPos().y)*Vector2(float(mouse.x-(this->window->width/2)), float(mouse.y -(this->window->height/2)));
		mousePos.y *= -1.0f;
		if (Input::IsKeyDown_DEBUG(Keyboard::Escape))
		{
			//Exit game
		}
		if (Input::IsKeyDown_DEBUG(Keyboard::E))
			deltaTime /= 4;


		/*if (Input::IsKeyDown_DEBUG(Keyboard::A))
			this->testObject->addRotation(Vector3(0.00f, 0.01f * deltaTime * 200, 0.00f));
		if(Input::IsKeyDown_DEBUG(Keyboard::D))
			this->testObject->addRotation(Vector3(0.00f, -0.01f * deltaTime * 200, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::W))
			this->testObject->addRotation(Vector3(0.01f * deltaTime * 200, 0.00f, 0.00f));
		if (Input::IsKeyDown_DEBUG(Keyboard::S))
			this->testObject->addRotation(Vector3(-0.01f * deltaTime * 200, 0.00f, 0.00f));*/
		
		player.update(deltaTime);
		this->camera.setPos(this->player.getVehicle()->getPosition() + Vector3(0, 20, 0));

		timerForParticle += deltaTime;
		if (timerForParticle > 0.01)
		{
			if (Input::IsKeyDown_DEBUG(Keyboard::C))
			{
				Vector2 dir = Input::GetDirectionR(0);
				this->graphics.addParticle(this->player.getVehicle()->getPosition() + Vector3(0, 1, 0.5f), Vector3(5 * dir.x, 0.0f, dir.y * 5));
				this->graphics.addParticle(this->player.getVehicle()->getPosition() + Vector3(0, 1, 0.3f), Vector3(5 * dir.x, 0.0f, dir.y * 5));
			}
			if (mouse.leftButton)
			{
				this->graphics.addParticle(Vector3(mousePos.x, 4, mousePos.y), 2 * Vector3(1.0f, 0.0f, 0.0f), addNrOfParticles, lifeTime);
			}
			this->graphics.addParticle(Vector3(0, 4, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

			timerForParticle = 0;
		}
		this->graphics.render(this->camera, deltaTime);
		//this->graphics.getdebugger()->DrawCube(this->testObject2->getTheAABB().maxPos, this->testObject2->getTheAABB().minPos,this->testObject2->getPosition(), Vector3(0, 1, 0));
		std::string textUse;


		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		//imgui button, slider etc
		ImGui::Begin("Gungame");
		ImGui::Text("frame time %.1f, %.1f FPS", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Hold 'V' To move camera with mouse.");
		ImGui::Text(("Mouse.x: " + std::to_string(mousePos.x)).c_str());
		ImGui::Text(("Mouse.y: " + std::to_string(mousePos.y)).c_str());
		Vector4 sdsa;
		
		Vector4 colorsP[4] =
		{
			Vector4(1.0f,1.0f,0.0f,0.5f),
			Vector4(1.0f,0.0f,0.0f,0.5f),
			Vector4(0.0f,0.0f,0.0f,0.5f),
			Vector4(0.1f,0.1f,0.1f,0.5f)
		};
		ImGui::Begin("particle");
		ImGui::ColorPicker4("Color Slider", colors);
		ImGui::ColorPicker4("Color 1 Slider", colors2);
		ImGui::ColorPicker4("Color 2 Slider", colors3);
		ImGui::ColorPicker4("Color 3 Slider", colors3);
		ImGui::SliderFloat("First size", &size1, 0.0f, 1.0f);
		ImGui::SliderFloat("Second size", &size2, 0.0f, 1.0f);
		ImGui::SliderInt("Nr of particles times 8", &addNrOfParticles, 1, 10);
		ImGui::SliderInt("LifeTime", &lifeTime, 1, 20);
		colorsP[0] = Vector4(colors);
		colorsP[1] = Vector4(colors2);
		colorsP[2] = Vector4(colors3);
		colorsP[3] = Vector4(colors4);

		this->graphics.setParticleColorNSize(colorsP, 4, size1, size2);
		ImGui::End();

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
		

		ImGui::End();

		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		
		player.update(deltaTime);
		//camera.setPos(player.getVehicle()->getPosition() + Vector3(0.0, 500.0f, 0.0));
		
		graphics.setSunVector(Vector3(sin(curTime * secPerCount * 0.1), cos(curTime * secPerCount * 0.1), -0.5));


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
}
