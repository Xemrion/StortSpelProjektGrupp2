#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include "Element.h"

class UserInterface
{
private:
	static std::unique_ptr<CommonStates> commonStates;
	static std::unique_ptr<SpriteBatch> spriteBatch;
	static std::unique_ptr<SpriteFont> fontArial;
public:
	static CommonStates* getCommonStates();
	static SpriteBatch* getSpriteBatch();
	static SpriteFont* getFontArial();
	static void init();

protected:
	virtual void updateUI(float deltaTime) = 0;
	virtual void drawUI() = 0;
private:
	void resetShaders();
public:
	UserInterface();
	virtual ~UserInterface();

	virtual void initUI() = 0;
	void update(float deltaTime);
};

#endif // !USER_INTERFACE_H
