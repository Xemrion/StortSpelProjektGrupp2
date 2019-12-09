#ifndef UI_PAUSED_H
#define UI_PAUSED_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/ButtonPromtBar.h"

class UIPaused : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Element* selected;

	std::unique_ptr<Button> buttonUnpause;
	std::unique_ptr<Button> buttonControls;
	std::unique_ptr<Button> buttonOptions;
	std::unique_ptr<Button> buttonMainMenu;
	std::unique_ptr<ButtonPromptBar> promptBar;
public:
	UIPaused();
	virtual ~UIPaused();

	void init();
};

#endif // !UI_PAUSED_H
