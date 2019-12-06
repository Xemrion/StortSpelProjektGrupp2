#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "Inventory/Container.h"
#include "Inventory/Item.h"
#include "States/MenuGameState.h"
#include "States/PlayingGameState.h"
#include "States/UpgradingGameState.h"
#include "States/HighscoreGameState.h"

std::unique_ptr<Game> Game::instance;

void Game::start(Window* window)
{
	instance = std::make_unique<Game>();
	instance->window = window;
	instance->graphics.init(window);

	Input::init(instance->window);
	Sound2::init();
	UserInterface::initStaticVariables();
	Container::playerInventory = std::make_unique<Container>();
	Item::init();

	Sound2::load("CarCrash.wav");
	Sound2::load("CarGlass1.wav");
	Sound2::load("CarGlass2.wav");
	Sound2::load("CarGlass3.wav");
	Sound2::load("CarGlass4.wav");
	Sound2::load("CarImpact1.wav");
	Sound2::load("CarImpact2.wav");
	Sound2::load("CarImpact3.wav");
	Sound2::load("CarImpact4.wav");
	Sound2::load("CarImpactSoft.wav");
	Sound2::load("FlameLoop1.wav");
	Sound2::load("FlameLoop2.wav");
	Sound2::load("HitSound.wav");
	Sound2::load("MachineGunSound1.wav");
	Sound2::load("MachineGunSound2.wav");
	Sound2::load("MachineGunSound3.wav");
	Sound2::load("MachineGunSound4.wav");
	Sound2::load("MachineGunSound5.wav");
	Sound2::load("MachineGunSound6.wav");
	Sound2::load("MetalImpact1.wav");
	Sound2::load("MetalImpact2.wav");
	Sound2::load("MetalImpact3.wav");
	Sound2::load("MetalImpactPitched1.wav");
	Sound2::load("MetalImpactPitched2.wav");
	Sound2::load("MetalImpactPitched3.wav");
		 
	Sound2::load("Drift2.mp3");
	Sound2::load("Drift1.mp3");
	Sound2::load("CarEngine1.mp3");
	Sound2::load("CarEngine2.mp3");
	Sound2::load("CarExhaust1.mp3");
	Sound2::load("CarExhaust2.mp3");
	Sound2::load("CarExhaust3.mp3");
	Sound2::load("CarExhaust4.mp3");
	Sound2::load("RobotBullet1.mp3");
	Sound2::load("RobotBullet2.mp3");
	Sound2::load("RobotBullet3.mp3");
	Sound2::load("RobotBullet4.mp3");
	Sound2::load("RobotBullet5.mp3");
	Sound2::load("RobotBullet6.mp3");
	Sound2::load("Lazer1.mp3");
	Sound2::load("Lazer2.mp3");
	Sound2::load("Lazer3.mp3");
	Sound2::load("Lazer4.mp3");
	Sound2::load("LazerImpact.mp3");


	

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

int Game::getCurrentStateIndex()
{
	return instance->currentState;
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

float Game::getDeltaTime()
{
	return instance->deltaTime;
}

float Game::getLocalScale()
{
	return instance->localScale;
}
GameInfo& Game::getGameInfo() noexcept
{
	return instance->gameInfo;
}

bool Game::getDrivingMode()
{
	return instance->drivingMode;
}

void Game::setDrivingMode(bool realistic)
{
	instance->drivingMode = realistic;
}


int Game::getNrOfStagesDone()
{
	return instance->gameInfo.nrOfClearedStages;
}



void Game::createCurrentState()
{
	VehicleSlots* newSlots = nullptr;

	if (oldState == STATE_PLAYING)
	{
		this->gameInfo.time = static_cast<PlayingGameState*>(state.get())->getTime();
	}

	if (currentState == STATE_MENU)
	{
		Sound2::stopAll();
		Sound2::playSoundtrack("OilSpillageSoundtrack1_Calm.mp3", "OilSpillageSoundtrack1_Aggressive.mp3");
		
		Container::playerInventory = std::make_unique<Container>();
		this->gameInfo = GameInfo();
		localScale = 1.0f;

		state = std::make_unique<MenuGameState>();
	}
	else if (currentState == STATE_PLAYING)
	{
		Sound2::stopAllLoops();
		Sound2::fadeSoundtrack(0.0f);
		gameInfo.highScoreStage = 0;

		if (oldState == STATE_UPGRADING)
		{
			newSlots = new VehicleSlots(*static_cast<UpgradingGameState*>(state.get())->getPlayer()->getSlots());
		}
		
		if (gameInfo.nrOfClearedStages <= 0)
		{
			// TODO: generate seed
			state = std::make_unique<PlayingGameState>(-1,7.0f*60.0f);
		}
		else
		{
			// TODO: use RNG with seed from above to generate subsequent map seeds
			state = std::make_unique<PlayingGameState>(-1,gameInfo.time);
		}

		gameInfo.nrOfClearedStages++;
		static_cast<PlayingGameState*>(state.get())->setCurrentMenu(PlayingGameState::MENU_BEFORE_PLAYING);
		static_cast<PlayingGameState*>(state.get())->generateObjectives();
		static_cast<PlayingGameState*>(state.get())->getPlayer()->startEngineSound();

		if (oldState == STATE_UPGRADING)
		{
			static_cast<PlayingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
		}
	}
	else if (currentState == STATE_UPGRADING)
	{
		Sound2::stopAllLoops();
		Sound2::fadeSoundtrack(0.0f);
		graphics.removeAllUIDraw();

		if (oldState == STATE_PLAYING)
		{
			localScale += 0.05f;
			newSlots = new VehicleSlots(*static_cast<PlayingGameState*>(state.get())->getPlayer()->getSlots());
		}
		else if (oldState == STATE_MENU)
		{
			newSlots = new VehicleSlots(*static_cast<MenuGameState*>(state.get())->getSlots());
		}

		state = std::make_unique<UpgradingGameState>();

		if (oldState == STATE_PLAYING || oldState == STATE_MENU)
		{
			static_cast<UpgradingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
		}
	}
	else if (currentState == STATE_HIGHSCORE)
	{
		Sound2::stopAllLoops();
		Sound2::fadeSoundtrack(0.0f);

		state = std::make_unique<HighscoreGameState>();
	}
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
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::update(deltaTime);
		Sound2::update(deltaTime);
		state->update(deltaTime);

		if (oldState != -1) {
			graphics.clearDraw();
			
			createCurrentState(); //Init the new state
			curTime = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
			oldState = -1;
		}
		

		//deltaTime reset
		prevTime = curTime;
	}
}

Game::Game() : currentState(STATE_MENU), oldState(-1), running(false) {}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}