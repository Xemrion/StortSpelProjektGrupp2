#include "game.h"

void Game::addQuad(int x)
{
	GameObject* object2 = new GameObject;
	object2->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(object2);
	object2->setPosition(glm::vec3(x, 0, 0));
}

void Game::init(Window* window)
{
	this->window = window; 
	graphics.init(window, 90);
	graphics.loadMesh("sda");
	for (int i = 0; i < 100; i++)
	{
		addQuad(i);
	}



}

void Game::run()
{
	while (this->window->update())
	{
		//Game logic
		//Graphics
		
		this->graphics.render();
	}

}
