#ifndef UPGRADING_GAME_STATE_H
#define UPGRADING_GAME_STATE_H

#include"../Vehicle.h"
#include "GameState.h"
#include "../game.h"
#include "../UI/UserInterface.h"

class UpgradingGameState : public GameState
{
public:
	enum Menu
	{
		MENU_RANDOMITEM,
		MENU_UPGRADING,
		MENUCOUNT
	};
private:
	Graphics& graphics;
	std::unique_ptr<UserInterface> menues[MENUCOUNT];
	std::unique_ptr<DynamicCamera> camera;
	std::unique_ptr<Vehicle> theVehicle;
	std::unique_ptr<LightList> lightList;
	std::unique_ptr<Physics> physics; //for vehicle
	std::unique_ptr<GameObject> background;
	std::unique_ptr<VehicleSlots> vehicleSlots;
	float timer;
	int currentMenu;
public:
	UpgradingGameState();
	virtual ~UpgradingGameState();

	Vehicle* getVehicle();
	void update(float deltaTime);
	void setCurrentMenu(Menu menu);

	std::unique_ptr<Vehicle> & getPlayer();
	void setPlayer(Vehicle* theVehicle);
};

#endif // !UPGRADING_GAME_STATE_H
