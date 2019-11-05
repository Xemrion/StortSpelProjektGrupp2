#ifndef UI_CREDITS_H
#define UI_CREDITS_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/CheckBox.h"
#include "../Elements/Slider.h"
#include <vector>
#include <string>

class UICredits : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> buttonBack;
	std::vector<std::string> creditsText;
public:
	UICredits();
	virtual ~UICredits();

	void init();
};

#endif // !UI_CREDITS_H
