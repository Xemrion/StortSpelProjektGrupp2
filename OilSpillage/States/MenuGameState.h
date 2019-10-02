#ifndef MENU_GAME_STATE_H
#define MENU_GAME_STATE_H

#include "GameState.h"
#include "../UI/UIMainMenu.h"

class MenuGameState : public GameState
{
private:
	std::unique_ptr<UIMainMenu> mainMenu;
public:
	MenuGameState();
	virtual ~MenuGameState();

	void init();
	void cleanUp();
	void update(float deltaTime);
};

#endif // !MENU_GAME_STATE_H
