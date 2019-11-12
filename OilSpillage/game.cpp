#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "Inventory/Inventory.h"
#include "Inventory/Item.h"
#include "States/MenuGameState.h"
#include "States/PlayingGameState.h"
#include "States/UpgradingGameState.h"

std::unique_ptr<Game> Game::instance;

void Game::start(Window* window)
{
	instance = std::make_unique<Game>();
	instance->window = window;
	instance->graphics.init(window);

	Input::init(instance->window);
	Sound::Init();
	UserInterface::initStaticVariables();
	Inventory::instance = std::make_unique<Inventory>();
	Item::init();

	Sound::loadSound(L"data/sound/OilSpillageSoundtrack1_Calm.wav");
	Sound::loadSound(L"data/sound/OilSpillageSoundtrack1_Aggressive.wav");

	Sound::PlayLoopingSound(L"data/sound/OilSpillageSoundtrack1_Calm.wav");
	Sound::PlayLoopingSound(L"data/sound/OilSpillageSoundtrack1_Aggressive.wav");
	Sound::changeVolume(L"data/sound/OilSpillageSoundtrack1_Aggressive.wav",0);

	Sound::loadSound(L"data/sound/CarCrash.wav");
	Sound::loadSound(L"data/sound/CarGlass1.wav");
	Sound::loadSound(L"data/sound/CarGlass2.wav");
	Sound::loadSound(L"data/sound/CarGlass3.wav");
	Sound::loadSound(L"data/sound/CarGlass4.wav");
	Sound::loadSound(L"data/sound/CarImpact1.wav");
	Sound::loadSound(L"data/sound/CarImpact2.wav");
	Sound::loadSound(L"data/sound/CarImpact3.wav");
	Sound::loadSound(L"data/sound/CarImpact4.wav");
	Sound::loadSound(L"data/sound/CarImpactSoft.wav");
	Sound::loadSound(L"data/sound/FlameLoop1.wav");
	Sound::loadSound(L"data/sound/FlameLoop2.wav");
	Sound::loadSound(L"data/sound/HitSound.wav");
	Sound::loadSound(L"data/sound/MachineGunSound1.wav");
	Sound::loadSound(L"data/sound/MachineGunSound2.wav");
	Sound::loadSound(L"data/sound/MachineGunSound3.wav");
	Sound::loadSound(L"data/sound/MachineGunSound4.wav");
	Sound::loadSound(L"data/sound/MachineGunSound5.wav");
	Sound::loadSound(L"data/sound/MachineGunSound6.wav");
	Sound::loadSound(L"data/sound/MetalImpact1.wav");
	Sound::loadSound(L"data/sound/MetalImpact2.wav");
	Sound::loadSound(L"data/sound/MetalImpact3.wav");
	Sound::loadSound(L"data/sound/MetalImpactPitched1.wav");
	Sound::loadSound(L"data/sound/MetalImpactPitched2.wav");
	Sound::loadSound(L"data/sound/MetalImpactPitched3.wav");

	//Input::setKeyboardPlayerID(0);
	instance->running = true;
	instance->run();
}

void Game::quit()
{
	instance->running = false;
}

GameState* Game::getCurrentState()
{
	return instance->state.get();
}

void Game::setState(State state)
{
	int newState = static_cast<int>(state);

	if (instance->oldState == -1 && instance->currentState != newState)
	{
		instance->oldState = instance->currentState;
		instance->currentState = newState;
	}
}

Graphics& Game::getGraphics() noexcept
{
	return instance->graphics;
}

float Game::lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

void Game::createCurrentState()
{
	if (currentState == STATE_MENU)
		state = std::make_unique<MenuGameState>();
	else if (currentState == STATE_PLAYING)
		state = std::make_unique<PlayingGameState>();
	else if (currentState == STATE_UPGRADING)
		state = std::make_unique<UpgradingGameState>();
}

void Game::run()
{
	//Init the state
	createCurrentState();

	//Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	//Initial previous time	
	prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);

	while (running && window->update()) {
		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::update();
		Sound::Update(deltaTime);
		state->update(deltaTime);

		if (oldState != -1) {
			graphics.clearDraw();
			createCurrentState(); //Init the new state
			curTime = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
			oldState = -1;
		}
		//camera.setPos(player.getVehicle()->getPosition() + Vector3(0.0, 500.0f, 0.0));
		

		//deltaTime reset
		prevTime = curTime;
	}
}

Game::Game() : currentState(STATE_UPGRADING), oldState(-1), running(false) {}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}