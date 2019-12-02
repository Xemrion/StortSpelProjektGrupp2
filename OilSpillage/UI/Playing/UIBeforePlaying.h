#ifndef UI_BEFORE_PLAYING_H
#define UI_BEFORE_PLAYING_H

#include "../UserInterface.h"
#include "../Elements/ButtonPromtBar.h"
#include "../../Texture.h"
#include "../../PG/Biome.hpp"
#include "../Elements/AnimatedText.h"

class UIBeforePlaying : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	Texture* textureBG;
	//Texture* textureWeather[5];
	std::unique_ptr<AnimatedText> textStage;
	std::unique_ptr<AnimatedText> textName;
	std::unique_ptr<AnimatedText> textSize;
	std::unique_ptr<AnimatedText> textBiome;
	std::unique_ptr<AnimatedText> textTime;
	std::unique_ptr<AnimatedText> textWeather;
	std::unique_ptr<ButtonPromptBar> promptBar;
public:
	UIBeforePlaying();
	virtual ~UIBeforePlaying();

	void init();
};

#endif // !UI_BEFORE_PLAYING_H
