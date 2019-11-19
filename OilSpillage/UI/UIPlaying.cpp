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
	this->minimap->update(deltaTime);
	this->objectiveBox->update(deltaTime);
}

void UIPlaying::drawUI()
{
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer();
	int time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	std::string timeStr = this->getFormattedTime();
	Vector2 textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
	Vector2 timeScale(0.8f, 0.8f);
	Vector2 shake;
	if (time < 11)
	{
		shake.x = rand() % 10 + 1.0f;
		shake.y = rand() % 10 + 1.0f;
		shake += Vector2(11.0f - time);
	}

	Vector2 position(Vector2(SCREEN_WIDTH / 2, textSize.y) + shake);
	Vector4 colorOverTime = Vector4(Colors::Yellow);
	if (time <= 20.0f)
	{
		colorOverTime = Vector4::Lerp(Vector4(Colors::Yellow), Vector4(Colors::Red), (20.0f - time) / 20.0f);
	}
	

	this->healthBar->setAmount(player->getHealth() / static_cast<float>(player->getMaxHealth()));

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	if (time <= 0)
	{
		timeStr = "Press Enter to Continue";
		std::string died = "Game over";
		timeScale = Vector2(0.7f, 0.7f);
		textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
		Vector2 diedTxtSize = UserInterface::getFontArial()->MeasureString(died.c_str());
		colorOverTime = Colors::Red;
		position = Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), died.c_str(), position, colorOverTime, 0, Vector2(diedTxtSize.x / 2, diedTxtSize.y / 2), timeScale);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), position+Vector2(0,textSize.y), colorOverTime, 0, Vector2(textSize.x / 2, textSize.y / 2), timeScale);

	}
	else
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), position+Vector2(0.0f,70.0f), colorOverTime, 0, Vector2(textSize.x / 2, (textSize.y / 2)+60), timeScale);
	}
	
	if (player->getRespawnTimer() > 0 && time > 0)
	{
		std::string respawnTime = std::to_string(int(player->getTotalRespawnTime() - player->getRespawnTimer()));
		Vector2 textSize = UserInterface::getFontArial()->MeasureString(respawnTime.c_str());
		if (this->respawnTimer >= 1.0f)
		{
			this->respawnTimer = 0.0f;
		}
		float sizeOverT = 0.1f + ((1 - this->respawnTimer) / 1)* (1.0f - 0.1f);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), respawnTime.c_str(), Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 ), Colors::White, 0, Vector2(textSize.x / 2, textSize.y / 2), sizeOverT);
		Vector2 textSize2 = UserInterface::getFontArial()->MeasureString("You died");

		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "You died", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)+Vector2(0,70.0f), Colors::Red, 0, Vector2(textSize2.x / 2, textSize2.y / 2), 1.0f);

	}
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Press Q to change driving mode", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(400, -340.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
	if (player->getDrivingMode() == 1) {
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Arcade", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(400, -300.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
	}else if (player->getDrivingMode() == 0) {
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Realistic", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(400, -300.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
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
	this->healthBar = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, 20));
	this->minimap = std::make_unique<Minimap>(0.25f, 25.0f, Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
	this->objectiveBox = std::make_unique<ObjectiveBox>(Vector2(-10, 15));
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}

bool UIPlaying::hasExploredOnMinimap(Vector3 worldPosition) const
{
	return this->minimap->hasExplored(worldPosition);
}
