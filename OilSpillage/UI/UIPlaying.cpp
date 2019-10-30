#include "UIPlaying.h"
#include "../Input.h"
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
	if (this->shouldInit)
	{
		this->minimap->init();
		this->objectiveBox->init();
		this->shouldInit = false;
	}

	this->respawnTimer += deltaTime;
	if (this->respawnTimer >= 1.0f)
	{
		this->respawnTimer = 0.0f;
	}

	this->minimap->update(deltaTime);
	this->objectiveBox->update(deltaTime);
}

void UIPlaying::drawUI()
{
	float time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	std::string timeStr = this->getFormattedTime();
	Vector2 textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
	Vector2 timeScale(1.0f, 1.0f);
	Vector2 shake(0, 0);
	if (time < 11)
	{
		shake.x = rand() % 10 + 1;
		shake.y = rand() % 10 + 1;
		shake += Vector2(11 - time);
	}
	Vector2 position(Vector2(SCREEN_WIDTH / 2, textSize.y) + shake);
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer();

	this->healthBar->setAmount(player->getHealth() / static_cast<float>(player->getMaxHealth()));
	
	Vector2 shake;
	if (time < 11)
	{
		shake.x = rand() % 10 + 1;
		shake.y = rand() % 10 + 1;
		shake += Vector2(11 - time);
	}

	Vector4 colorOverTime = Vector4(Colors::Yellow);
	if (time <= 20.0f)
	{
		colorOverTime = Vector4::Lerp(Vector4(Colors::Yellow), Vector4(Colors::Red), (20.0f - time) / 20.0f);
	}
	if (time <= 0)
	{
		timeStr = "Game Over Press Enter to Continue";
		textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
		timeScale = Vector2(0.7f, 0.7f);
		position = Vector2(SCREEN_WIDTH / 2, 720 / 2);
	}

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 10) + shake, colorOverTime);
	
	if (player->getRespawnTimer() > 0)
	{
		std::string respawnTime = std::to_string(int(player->getTotRespawnTime() - player->getRespawnTimer()));
		Vector2 textSize = UserInterface::getFontArial()->MeasureString(respawnTime.c_str());
		float sizeOverT = 0.1f + ((1 - this->respawnTimer) / 1)* (1.0f - 0.1f);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), respawnTime.c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, SCREEN_HEIGHT / 2 - textSize.y / 2), Colors::White, 0, Vector2(0, 0), sizeOverT);
	}

	this->healthBar->draw(false);
	this->objectiveBox->draw(false);
	this->minimap->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIPlaying::UIPlaying() : shouldInit(true), respawnTimer(0)
{
}

UIPlaying::~UIPlaying()
{
}

void UIPlaying::init()
{
	this->healthBar = std::make_unique<Slider>(Vector2(10, 10));
	this->minimap = std::make_unique<Minimap>(0.25f, 25.0f, Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
	this->objectiveBox = std::make_unique<ObjectiveBox>(Vector2(0, 50));
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}
