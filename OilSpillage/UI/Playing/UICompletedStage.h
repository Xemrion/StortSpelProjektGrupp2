#ifndef UI_COMPLETED_STAGE_H
#define UI_COMPLETED_STAGE_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/AnimatedText.h"
#include "../Elements/ButtonPromtBar.h"

class UICompletedStage : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Element* selected;

	float timer;
	std::unique_ptr<AnimatedText> textResults;
	std::unique_ptr<AnimatedText> textStats[8];
	std::unique_ptr<Button> buttonSaveContinue;
	std::unique_ptr<Button> buttonSaveExit;
	std::unique_ptr<ButtonPromptBar> promptBar;
public:
	UICompletedStage();
	virtual ~UICompletedStage();

	void init();
};

#endif // !UI_COMPLETED_STAGE_H
