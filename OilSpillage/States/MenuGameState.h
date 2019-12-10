#ifndef MENU_GAME_STATE_H
#define MENU_GAME_STATE_H

#include "GameState.h"
#include "../game.h"
#include "../UI/UserInterface.h"
#include "../Vehicle.h"

class MenuGameState : public GameState
{
public:
	enum Menu
	{
		MENU_MAIN,
		MENU_OPTIONS,
		MENU_CREDITS,
		MENU_CONTROLS,
		MENU_HIGHSCORE,
		MENU_LOAD,
		MENUCOUNT
	};
private:
	Graphics& graphics;
	std::unique_ptr<UserInterface> menues[MENUCOUNT];
	int currentMenu;
	int oldMenu;
	Texture* textureLogo;
	
	std::unique_ptr<VehicleSlots> slots;
	std::unique_ptr<Physics> physics;
	std::unique_ptr<Vehicle> theVehicle;
	std::unique_ptr<LightList> lightList;
	std::unique_ptr<DynamicCamera> camera;
	std::unique_ptr<GameObject> barrels;
public:
	MenuGameState();
	virtual ~MenuGameState();

	void update(float deltaTime);
	void setCurrentMenu(Menu menu);
	VehicleSlots* getSlots();
};

#endif // !MENU_GAME_STATE_H
