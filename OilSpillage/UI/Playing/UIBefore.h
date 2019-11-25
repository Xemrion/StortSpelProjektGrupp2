#ifndef UI_BEFORE_H
#define UI_BEFORE_H

#include "../UserInterface.h"
#include "../../Texture.h"

class UIBefore : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Texture* textureSun;
	Texture* textureRain;
	Texture* textureStorm;
	Texture* textureSnow;
	Texture* textureSandstorm;
public:
	UIBefore();
	virtual ~UIBefore();

	void init();
};

#endif // !UI_BEFORE_H
