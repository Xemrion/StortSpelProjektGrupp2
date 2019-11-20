#ifndef UI_BEFORE_H
#define UI_BEFORE_H

#include "../UserInterface.h"
#include "../../Texture.h"

class UIBefore : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	/*Texture* textureSun;
	Texture* textureSun;
	Texture* textureSun;
	Texture* textureSun;
	Texture* textureSun;
	Texture* textureSun;*/
public:
	UIBefore();
	virtual ~UIBefore();

	void init();
};

#endif // !UI_BEFORE_H
