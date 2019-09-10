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
	graphics.loadMesh("sda");
	
	this->testObject = new GameObject;
	testObject->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(testObject);
	testObject->setPosition(Vector3(2, 0, 0));
	testObject->setScale(Vector3(2, 2, 2));
}

void Game::run()
{
	while (this->window->update())
	{
		//Game logic
		//Graphics
		
		this->graphics.render();
		this->testObject->addRotation(Vector3(0.001, 0.01, 0));
	}

}
