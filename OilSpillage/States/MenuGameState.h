#ifndef MENU_GAME_STATE_H
#define MENU_GAME_STATE_H

#include "GameState.h"
#include "../game.h"
#include "../UI/UserInterface.h"
#include "../Texture.h"

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
		MENUCOUNT
	};
private:
	Graphics& graphics;
	std::unique_ptr<UserInterface> menues[MENUCOUNT];
	int currentMenu;
	Texture* textureBG;
public:
	MenuGameState();
	virtual ~MenuGameState();

	void update(float deltaTime);
	void setCurrentMenu(Menu menu);
};

#endif // !MENU_GAME_STATE_H
