#include "game.h"

void Game::init(Window* window)
{
	this->window = window;
	GameObject* object= new GameObject;
	graphics.loadMesh("sda");
	object->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(object);
	graphics.init(window, 90);
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
