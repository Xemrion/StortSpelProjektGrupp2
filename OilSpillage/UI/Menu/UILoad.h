#ifndef UI_LOAD_H
#define UI_LOAD_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/ButtonPromtBar.h"
#include "../Elements/AnimatedText.h"

class UILoad : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Element* selected;

	bool erasing;
	std::unique_ptr<AnimatedText> textFileSelect;
	std::unique_ptr<Button> buttonFile[5];
	std::unique_ptr<Button> buttonErase;
	std::unique_ptr<Button> buttonBack;
	std::unique_ptr<ButtonPromptBar> promptBar;
public:
	UILoad();
	virtual ~UILoad();

	void init();
};

#endif // !UI_LOAD_H
