#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include "Elements/Element.h"

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
	static void initStaticVariables();
	static Element* changeSelected(Element* selected);

protected:
	virtual void updateUI(float deltaTime) = 0;
	virtual void drawUI() = 0;
private:
	void resetShaders() const;
public:
	UserInterface();
	virtual ~UserInterface();

	virtual void init() = 0;
	void update(float deltaTime);
};

#endif // !USER_INTERFACE_H
