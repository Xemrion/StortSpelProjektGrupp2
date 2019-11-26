#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "Inventory/Container.h"
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
	Sound::init();
	UserInterface::initStaticVariables();
	Container::playerInventory = std::make_unique<Container>();
	Item::init();

	Sound::load("./data/sound/CarCrash.wav");
	Sound::load("./data/sound/CarGlass1.wav");
	Sound::load("./data/sound/CarGlass2.wav");
	Sound::load("./data/sound/CarGlass3.wav");
	Sound::load("./data/sound/CarGlass4.wav");
	Sound::load("./data/sound/CarImpact1.wav");
	Sound::load("./data/sound/CarImpact2.wav");
	Sound::load("./data/sound/CarImpact3.wav");
	Sound::load("./data/sound/CarImpact4.wav");
	Sound::load("./data/sound/CarImpactSoft.wav");
	Sound::load("./data/sound/FlameLoop1.wav");
	Sound::load("./data/sound/FlameLoop2.wav");
	Sound::load("./data/sound/HitSound.wav");
	Sound::load("./data/sound/MachineGunSound1.wav");
	Sound::load("./data/sound/MachineGunSound2.wav");
	Sound::load("./data/sound/MachineGunSound3.wav");
	Sound::load("./data/sound/MachineGunSound4.wav");
	Sound::load("./data/sound/MachineGunSound5.wav");
	Sound::load("./data/sound/MachineGunSound6.wav");
	Sound::load("./data/sound/MetalImpact1.wav");
	Sound::load("./data/sound/MetalImpact2.wav");
	Sound::load("./data/sound/MetalImpact3.wav");
	Sound::load("./data/sound/MetalImpactPitched1.wav");
	Sound::load("./data/sound/MetalImpactPitched2.wav");
	Sound::load("./data/sound/MetalImpactPitched3.wav");
	
	Sound::load("./data/sound/Drift2.mp3");
	Sound::load("./data/sound/Drift1.mp3");
	Sound::load("./data/sound/CarEngine1.mp3");
	Sound::load("./data/sound/CarEngine2.mp3");
	Sound::load("./data/sound/CarExhaust1.mp3");
	Sound::load("./data/sound/CarExhaust2.mp3");
	Sound::load("./data/sound/CarExhaust3.mp3");
	Sound::load("./data/sound/CarExhaust4.mp3");
	Sound::load("./data/sound/RobotBullet1.mp3");
	Sound::load("./data/sound/RobotBullet2.mp3");
	Sound::load("./data/sound/RobotBullet3.mp3");
	Sound::load("./data/sound/RobotBullet4.mp3");
	Sound::load("./data/sound/RobotBullet5.mp3");
	Sound::load("./data/sound/RobotBullet6.mp3");
	Sound::load("./data/sound/Lazer1.mp3");
	Sound::load("./data/sound/Lazer2.mp3");
	Sound::load("./data/sound/Lazer3.mp3");
	Sound::load("./data/sound/Lazer4.mp3");
	Sound::load("./data/sound/LazerImpact.mp3");


	Sound::playSoundtrack("./data/sound/OilSpillageSoundtrack1_Calm.mp3", "./data/sound/OilSpillageSoundtrack1_Aggressive.mp3");

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
	// TODO: insert return statement here
}


int Game::getNrOfStagesDone()
{
	return instance->nrOfStagesDone;
}



void Game::createCurrentState()
{
	VehicleSlots* transfer = nullptr;
	VehicleSlots* newSlots = nullptr;
	Vehicle*temp = nullptr;
	if (currentState == STATE_MENU)
	{
		Container::playerInventory = std::make_unique<Container>();

		nrOfStagesDone = 0.0f;
		localScale = 1.0f;
		state = std::make_unique<MenuGameState>();
		Sound::stopAllSoundsExceptSoundtrack();
	}
	else if (currentState == STATE_PLAYING)
	{
		if (oldState == STATE_UPGRADING)
		{
			transfer = static_cast<UpgradingGameState*>(state.get())->getPlayer()->getSlots();
			newSlots = new VehicleSlots(*transfer);
			temp = static_cast<UpgradingGameState*>(state.get())->getPlayer().get();
			temp->stopEngineSound();
			nrOfStagesDone++;
			Sound::stopAllSoundsExceptSoundtrack();
			localScale += 0.05f; // increase everytime a new stage is created
			
		}
		state = std::make_unique<PlayingGameState>();

		if (oldState == STATE_UPGRADING)
		{	
			static_cast<PlayingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
			Sound::stopAllSoundsExceptSoundtrack();
			//static_cast<PlayingGameState*>(state.get())->initiatePlayer();
		}
	}
	else if (currentState == STATE_UPGRADING)
	{
		graphics.removeAllUIDraw();
		if (oldState == STATE_PLAYING)
		{
			transfer = static_cast<PlayingGameState*>(state.get())->getPlayer()->getSlots();
			newSlots = new VehicleSlots(*transfer);
			temp = static_cast<PlayingGameState*>(state.get())->getPlayer().get();
			temp->stopEngineSound();
			Sound::stopAllSoundsExceptSoundtrack();
		}

		state = std::make_unique<UpgradingGameState>();

		if (oldState == STATE_PLAYING)
		{
			Sound::stopAllSoundsExceptSoundtrack();
			static_cast<UpgradingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
			
		}
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
		//deltaTime
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
		//Calculate deltaTime
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::update(deltaTime);
		Sound::update(deltaTime);
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
	Sound::stopAll();
	Sound::deinit();
}

Game::Game() : currentState(STATE_MENU), oldState(-1), running(false) {}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}