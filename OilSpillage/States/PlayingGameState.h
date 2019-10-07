#ifndef PLAYING_GAME_STATE_H
#define PLAYING_GAME_STATE_H

#include "GameState.h"
#include "../game.h"
#include "../ImGui/imgui.h"
#include "../vehicle.h"
#include "../AI/Actor.h"
#include "../Road Network/RoadNet.h"
#include "../DynamicCamera.h"
#include "../PG/Map.hpp"
#include <array>

class PlayingGameState : public GameState
{
private:
	friend class Game;

	Graphics& graphics;
	std::unique_ptr<Map> map;
	std::unique_ptr<AIPlayer> aiObject;
	std::vector<GameObject> markers;
	std::vector<GameObject> tiles;
	std::unique_ptr<LightList> lightList;
	std::unique_ptr<Vehicle> player;
	std::unique_ptr<DynamicCamera> camera;
	std::vector<CinematicPos> points;
	SpotLight* playerLight = nullptr;
	std::unique_ptr<RoadNetwork> testNetwork;
	//AStar aStar; TODO

	void generateMap();
	void initiateAStar();
public:
	PlayingGameState();
	virtual ~PlayingGameState();

	void init();
	void cleanUp();
	void update(float deltaTime);
};

#endif // !PLAYING_GAME_STATE_H
