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
	std::unique_ptr<Slider> sliderMaster;
	std::unique_ptr<Slider> sliderMusic;
	std::unique_ptr<Slider> sliderEffects;
	std::unique_ptr<ButtonPromptBar> promptBar;
	Element* selected;

public:
	UIOptions();
	virtual ~UIOptions();

	void init();

	virtual void unloadTextures();
};

#endif // !UI_OPTIONS_H
