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
}

void UIPlaying::drawUI()
{
	this->objHandlerPtr = &static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler();
	std::string timeStr = this->getFormattedTime();
	Vector2 textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer();
	this->healthBar->setAmount(player->getHealth() / static_cast<float>(player->getMaxHealth()));
	

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 10), Colors::LightPink);
	
	
	Color color;
	std::string infoUI;
	if (objHandlerPtr->getObjective(0)==nullptr)
	{
		infoUI = " You did it. Get out now!!! ";
		color = Colors::Yellow;
	}
	else
	{
		infoUI = objHandlerPtr->getObjective(0)->getInfo();
		color = Colors::White;
	}

	
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(),infoUI.c_str(), Vector2(0, 40), color,0,Vector3(0,0,0),Vector3(0.2,0.2,0.2));

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
	this->minimap = std::make_unique<Minimap>(0.25f, Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
}
