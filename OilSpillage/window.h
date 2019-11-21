#pragma once
#include <Windows.h>

class Window {
public:
	HWND handle;
	int width;
	int height;
	static bool resized;
	bool init(HINSTANCE hInstance, int width, int height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool update();
	void shutdown();
};
