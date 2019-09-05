#include "game.h"

void Game::init(Window* window)
{
	this->window = window;
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
