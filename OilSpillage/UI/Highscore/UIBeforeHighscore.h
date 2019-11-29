#ifndef UI_BEFORE_HIGHSCORE_H
#define UI_BEFORE_HIGHSCORE_H

#include "../UserInterface.h"
#include "../../Texture.h"
#include "../Elements/AnimatedText.h"
#include "../Elements/ButtonPromtBar.h"

class UIBeforeHighscore : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::string name;
	std::unique_ptr<ButtonPromptBar> promptBar;
	std::unique_ptr<AnimatedText> yourName;
	std::unique_ptr<AnimatedText> theName;
public:
	UIBeforeHighscore();
	virtual ~UIBeforeHighscore();

	void init();
	std::string getName() const;
};

#endif // !UI_BEFORE_HIGHSCORE_H
