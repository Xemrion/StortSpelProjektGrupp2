#include "game.h"
#include <Windows.h>
#include <ctime>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(static_cast<unsigned int>(time(NULL)));
	Game game;
	Window window;
	window.init(hInstance, 1280, 720);
	ShowWindow(window.handle, nShowCmd);
	game.init(&window);
	game.run();
	return 0;
}
