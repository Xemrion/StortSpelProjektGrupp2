#include "game.h"

void Game::init(Window* window)
{
	this->window = window;
	graphics.init(window);
}

void Game::run()
{
	while (this->window->update())
	{
		//Game logic
		//Graphics
	}

}
