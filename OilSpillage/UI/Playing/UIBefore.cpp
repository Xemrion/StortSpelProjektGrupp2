#include "UIBefore.h"
#include "../../game.h"
#include <cassert>

void UIBefore::updateUI(float deltaTime)
{
}

void UIBefore::drawUI()
{
	SpriteBatch* sb = UserInterface::getSpriteBatch();
	SpriteFont* font = UserInterface::getFontArial();

	font->DrawString(sb, "test", Vector2::Zero);
}

UIBefore::UIBefore() : textureSun(nullptr), textureRain(nullptr), textureStorm(nullptr), textureSnow(nullptr), textureSandstorm(nullptr)
{
}

UIBefore::~UIBefore()
{
}

void UIBefore::init()
{
	Game::getGraphics().loadTexture("UI/weather_sun");
	Game::getGraphics().loadTexture("UI/weather_rain");
	Game::getGraphics().loadTexture("UI/weather_storm");
	Game::getGraphics().loadTexture("UI/weather_snow");
	Game::getGraphics().loadTexture("UI/weather_sandstorm");

	this->textureSun = Game::getGraphics().getTexturePointer("UI/weather_sun");
	this->textureRain = Game::getGraphics().getTexturePointer("UI/weather_rain");
	this->textureStorm = Game::getGraphics().getTexturePointer("UI/weather_storm");
	this->textureSnow = Game::getGraphics().getTexturePointer("UI/weather_snow");
	this->textureSandstorm = Game::getGraphics().getTexturePointer("UI/weather_sandstorm");

	assert(textureSun && "Texture failed to load!");
	assert(textureRain && "Texture failed to load!");
	assert(textureStorm && "Texture failed to load!");
	assert(textureSnow && "Texture failed to load!");
	assert(textureSandstorm && "Texture failed to load!");
}
