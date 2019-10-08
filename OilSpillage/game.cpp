#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "States/MenuGameState.h"
#include "States/PlayingGameState.h"

Graphics Game::graphics = Graphics();

std::unique_ptr<GameState> Game::states[STATECOUNT];
int Game::currentState = STATE_MENU;
int Game::oldState = -1;

GameState* Game::getCurrentState()
{
	return states[currentState].get();
}

void Game::setState(State state)
{
	int newState = static_cast<int>(state);

	if (oldState == -1 && currentState != newState)
	{
		oldState = currentState;
		currentState = newState;
	}
}

Game::Game()
{
	states[STATE_MENU]    = std::make_unique<MenuGameState>();
	states[STATE_PLAYING] = std::make_unique<PlayingGameState>();
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::init(Window* window)
{
	this->window = window; 
	graphics.init(window);
	Input::Init();
	Sound::Init();
	UserInterface::initStaticVariables();
}

void Game::run()
{
	//Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	//Initial previous time	
	prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*) &prevTime);

	Input::SetKeyboardPlayerID(0);
	states[currentState]->init();

	while (window->update()) {
		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*) &curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::Update();
		Sound::Update(deltaTime);
		states[currentState]->update(deltaTime);

		if (oldState != -1) {
			states[oldState]->cleanUp();
			graphics.clearDraw();
			states[currentState]->init();
			oldState = -1;
		}

		//deltaTime reset
		prevTime = curTime;
	}
}  