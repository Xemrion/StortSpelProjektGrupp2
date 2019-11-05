#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/CheckBox.h"
#include "../Elements/Slider.h"

class UIMainMenu : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> buttonPlay;
	std::unique_ptr<Button> buttonOptions;
	std::unique_ptr<Button> buttonCredits;
	std::unique_ptr<Button> buttonQuit;
	Element* selected;

public:
	UIMainMenu();
	virtual ~UIMainMenu();

	void init();
};

#endif // !UI_MAIN_MENU_H
