#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H

#include "UserInterface.h"

class UIMainMenu : public UserInterface
{
private:
	Element** elements;
	int nrOfElements;

public:
	UIMainMenu();
	virtual ~UIMainMenu();

	void init();
};

#endif // !UI_MAIN_MENU_H
