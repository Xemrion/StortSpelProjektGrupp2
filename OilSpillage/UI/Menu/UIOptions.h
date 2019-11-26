#ifndef UI_OPTIONS_H
#define UI_OPTIONS_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/CheckBox.h"
#include "../Elements/Slider.h"
#include "../Elements/ButtonPromtBar.h"

class UIOptions : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> buttonBack;
	std::unique_ptr<CheckBox> checkBoxDrivingMode;
	std::unique_ptr<Slider> sliderTest;
	std::unique_ptr<ButtonPromptBar> promptBar;
	Element* selected;

public:
	UIOptions();
	virtual ~UIOptions();

	void init();
};

#endif // !UI_OPTIONS_H
