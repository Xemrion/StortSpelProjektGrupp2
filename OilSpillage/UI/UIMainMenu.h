#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H

#include "UserInterface.h"
#include "Button.h"

class UIMainMenu : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> button;

public:
	UIMainMenu();
	virtual ~UIMainMenu();

	void initUI();
};

#endif // !UI_MAIN_MENU_H
