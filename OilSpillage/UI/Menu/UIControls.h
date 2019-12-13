#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include "../UserInterface.h"
#include "../../Texture.h"
#include "../Elements/ButtonPromtBar.h"

class UIControls : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Texture* textureControls;
	std::unique_ptr<ButtonPromptBar> promptBar;
	std::unique_ptr<ButtonPromptBar> promptBar2;
public:
	UIControls();
	virtual ~UIControls();

	void init();

	virtual void unloadTextures();
};

#endif // !UI_CONTROLS_H
