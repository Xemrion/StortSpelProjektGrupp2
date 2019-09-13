#include "game.h"
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	Game game;
	Window window;
	window.init(hInstance, 1280, 720);
	ShowWindow(window.handle, nShowCmd);
	game.init(&window);
	game.run();
	return 0;
}
