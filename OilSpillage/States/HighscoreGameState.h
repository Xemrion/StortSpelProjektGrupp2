#ifndef HIGHSCORE_GAME_STATE_H
#define HIGHSCORE_GAME_STATE_H

#include "GameState.h"
#include "../game.h"
#include "../UI/UserInterface.h"
#include "../Texture.h"
#include "../Vehicle.h"
#include <vector>

class HighscoreGameState : public GameState
{
public:
	enum Menu
	{
		MENU_BEFORE,
		MENU_HIGHSCORE,
		MENUCOUNT
	};
private:
	Graphics& graphics;
	std::unique_ptr<UserInterface> menues[MENUCOUNT];
	int currentMenu;

	std::unique_ptr<Physics> physics; //for vehicle
	std::unique_ptr<Vehicle> theVehicle;
	std::unique_ptr<LightList> lightList;
	std::unique_ptr<DynamicCamera> camera;

	std::unique_ptr<GameObject> barrels;
public:
	HighscoreGameState();
	virtual ~HighscoreGameState();

	void update(float deltaTime);
	void setCurrentMenu(Menu menu);
};

#endif // !HIGHSCORE_GAME_STATE_H
