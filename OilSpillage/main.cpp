#include "game.h"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Game game;
	Window window;
	window.init(hInstance, 1280, 720);
	ShowWindow(window.handle, nCmdShow);
	game.init(&window);
	game.run();
	return 0;
}
