#ifndef UI_BEFORE_H
#define UI_BEFORE_H

#include "../UserInterface.h"
#include "../../Texture.h"
#include "../Elements/AnimatedText.h"
#include "../Elements/ButtonPromtBar.h"

class UIBefore : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::string name;
	std::unique_ptr<ButtonPromptBar> promptBar;
	std::unique_ptr<AnimatedText> yourName;
	std::unique_ptr<AnimatedText> theName;
public:
	UIBefore();
	virtual ~UIBefore();

	void init();
	std::string getName() const;
};

#endif // !UI_BEFORE_H
