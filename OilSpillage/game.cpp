#include "game.h"

void Game::init(Window* window)
{
	this->window = window;
	graphics.init(window);
}

void Game::run()
{
	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}
