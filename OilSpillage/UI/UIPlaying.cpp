#include "UIPlaying.h"
#include "..//Input.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include <sstream>

std::string UIPlaying::getFormattedTime()
{
	int time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	int minutes = time / 60;
	int seconds = time % 60;

	std::stringstream timeStrStream;

	if (minutes < 10)
		timeStrStream << "0";

	timeStrStream << minutes;
	timeStrStream << ":";

	if (seconds < 10)
		timeStrStream << "0";

	timeStrStream << seconds;

	return timeStrStream.str();
}

void UIPlaying::updateUI(float deltaTime)
{
	if (this->initMinimap)
	{
		this->minimap->init();
		this->initMinimap = false;
	}

	this->minimap->update(deltaTime);
}

void UIPlaying::drawUI()
{
	std::string timeStr = this->getFormattedTime();
	Vector2 textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer();

	this->healthBar->setAmount(player->getHealth() / static_cast<float>(player->getMaxHealth()));

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 10), Colors::LightPink);
	this->healthBar->draw(false);
	this->minimap->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIPlaying::UIPlaying() : initMinimap(true)
{
}

UIPlaying::~UIPlaying()
{
}

void UIPlaying::init()
{
	this->healthBar = std::make_unique<Slider>(Vector2(10, 10));
	this->minimap = std::make_unique<Minimap>(0.25f, 20.0f, Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}
