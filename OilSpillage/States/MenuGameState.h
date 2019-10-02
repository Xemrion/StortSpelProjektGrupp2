#ifndef MENU_GAME_STATE_H
#define MENU_GAME_STATE_H

#include "GameState.h"
#include "../game.h"
#include "../UI/UserInterface.h"

enum Menu
{
	MENU_MAIN,
	MENU_OPTIONS,
	MENU_CREDITS,
	MENUCOUNT
};

class MenuGameState : public GameState
{
private:
	Graphics& graphics;
	std::unique_ptr<UserInterface> menues[MENUCOUNT];
	int currentMenu;
public:
	MenuGameState();
	virtual ~MenuGameState();

	void init();
	void cleanUp();
	void update(float deltaTime);

	void setCurrentMenu(Menu menu);
};

#endif // !MENU_GAME_STATE_H
