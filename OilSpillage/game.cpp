#include "game.h"
Graphics Game::graphics = Graphics();

void Game::addQuad(int x)
{
	GameObject* object2 = new GameObject;
	object2->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(object2);
	object2->setPosition(Vector3(x, 0, 0));
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
	graphics.loadTexture("playerRabbit.tga");
	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(0, 0, 0));
	testObject->setScale(Vector3(0.2, 0.2, 0.2));
	testObject->setColor(Vector4(0.5, 0.5, 0.5,1));
	testObject->setTexture(graphics.getTexturePointer("playerRabbit.tga"));
	testObject->setColor(Vector4(1, 1, 1, 1));
	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(-7, 0, 0));
	testObject2->setColor(Vector4(0.5, 0.5, 0.5, 1));
	testObject2->setTexture(graphics.getTexturePointer("playerRabbit.tga"));

	player.init(this->graphics);
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
			this->testObject->addRotation(Vector3(0.00, 0.01*deltaTime*200, 0.00));
		if(kb.D)
			this->testObject->addRotation(Vector3(0.00, -0.01 * deltaTime * 200, 0.00));
		if (kb.W)
			this->testObject->addRotation(Vector3(0.01 * deltaTime * 200, 0.00, 0.00));
		if (kb.S)
			this->testObject->addRotation(Vector3(-0.01 * deltaTime * 200, 0.00, 0.00));
		
		player.update(deltaTime, this->keyboard);
		
		this->graphics.render();
		
		//deltaTime reset
		prevTime = curTime;
	}
	delete this->testObject;
	delete this->testObject2;
}
