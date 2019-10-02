#ifndef UI_OPTIONS_H
#define UI_OPTIONS_H

#include "UserInterface.h"
#include "Button.h"
#include "CheckBox.h"
#include "Slider.h"

class UIOptions : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> buttonBack;
	std::unique_ptr<CheckBox> checkBoxTest;
	std::unique_ptr<Slider> sliderTest;
	Element* selected;

public:
	UIOptions();
	virtual ~UIOptions();

	void init();
};

#endif // !UI_OPTIONS_H
