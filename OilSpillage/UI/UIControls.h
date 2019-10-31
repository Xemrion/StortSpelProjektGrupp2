#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include "UserInterface.h"
#include "../Texture.h"

class UIControls : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Texture* textureControls;
public:
	UIControls();
	virtual ~UIControls();

	void init();
};

#endif // !UI_CONTROLS_H
