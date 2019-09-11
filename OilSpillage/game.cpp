#include "game.h"

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
	testObject->setScale(Vector3(2, 2, 2));
	testObject->setTexture(graphics.getTexturePointer("playerRabbit.tga"));

	this->testObject2 = new GameObject;
	testObject2->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(testObject2);
	testObject2->setPosition(Vector3(-7, 0, 0));
	testObject2->setTexture(graphics.getTexturePointer("playerRabbit.tga"));

}

void Game::run()
{
	while (this->window->update())
	{
		//Game logic
		//Graphics
		auto kb = this->keyboard->GetState();
		if (kb.Escape)
		{
			//Exit game
		}
		if (kb.A)
			this->testObject->addRotation(Vector3(0.00, 0.01, 0.00));
		if(kb.D)
			this->testObject->addRotation(Vector3(0.00, -0.01, 0.00));
		if (kb.W)
			this->testObject->addRotation(Vector3(0.01, 0.00, 0.00));
		if (kb.S)
			this->testObject->addRotation(Vector3(-0.01, 0.00, 0.00));
		auto mouse = this->mouse->GetState();
		this->graphics.render();
		
		
	}
	delete this->testObject;
	delete this->testObject2;
}
