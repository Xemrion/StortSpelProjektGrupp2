#include "game.h"
Graphics Game::graphics = Graphics();

void Game::addQuad(int x)
{
	GameObject* object2 = new GameObject;
	object2->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(object2);
	object2->setPosition(Vector3(static_cast<float>(x), 0.0f, 0.0f));
}

void Game::init(Window* window)
{
	this->window = window; 
	graphics.init(window, 90);
	
	this->keyboard = std::make_unique<Keyboard>();
	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow(window->handle);
	graphics.loadMesh("sda");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("brickwall.tga");
	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	testObject->setScale(Vector3(0.2f, 0.2f, 0.2f));
	//testObject->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject->setTexture(graphics.getTexturePointer("brickwall.tga"));
	//testObject->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(-7.0f, 0.0f, 0.0f));
	//testObject2->setColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	testObject2->setTexture(graphics.getTexturePointer("brickwall.tga"));
	
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


	while (this->window->update())
	{
		//Game logic
		//Graphics

		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;



		auto kb = this->keyboard->GetState();
		auto mouse = this->mouse->GetState();
		if (kb.Escape)
		{
			//Exit game
		}
		if (kb.E)
			deltaTime /= 4;


		if (kb.A)
			this->testObject->addRotation(Vector3(0.00f, 0.01f * deltaTime * 200, 0.00f));
		if(kb.D)
			this->testObject->addRotation(Vector3(0.00f, -0.01f * deltaTime * 200, 0.00f));
		if (kb.W)
			this->testObject->addRotation(Vector3(0.01f * deltaTime * 200, 0.00f, 0.00f));
		if (kb.S)
			this->testObject->addRotation(Vector3(-0.01f * deltaTime * 200, 0.00f, 0.00f));
		
		player.update(deltaTime, this->keyboard);
		
		this->graphics.render();
		
		//deltaTime reset
		prevTime = curTime;
	}
	delete this->testObject;
	delete this->testObject2;
}
