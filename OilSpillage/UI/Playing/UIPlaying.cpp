#include "UIPlaying.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"
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

	int time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	Color color(Colors::Yellow);

	int secondsFromMinute = static_cast<int>(std::roundf(time)) % 60;
	if (secondsFromMinute <= 1 || secondsFromMinute >= 59 || time <= 10.0f)
	{
		timerScale = Game::lerp(timerScale, 1.0f, deltaTime * 2);
		color = Color(Colors::GreenYellow);
	}
	else
	{
		timerScale = Game::lerp(timerScale, 0.5f, deltaTime * 2);
		color = Color(Colors::Yellow);
	}

	if (time <= 20.0f)
	{
		color = Color::Lerp(Color(Colors::Red), Color(Colors::Yellow), time / 20.0f);
	}

	if (time <= 10.0f)
	{
		this->timer->setAnimation(Animation::SHAKING);
	}

	this->timer->setVariables(this->getFormattedTime(), color, timerScale);
	this->timer->setPosition(Vector2(SCREEN_WIDTH / 2 - this->timer->getSize().x / 2, 20 + Slider::size.y + 10));
	this->timer->update(deltaTime);
}

void UIPlaying::drawUI()
{
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer().get();
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
	this->timer->draw(false);
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), std::to_string(Game::getGameInfo().highScore).c_str(), Vector2(0, 0), Colors::Red, 0, Vector2::Zero, 1.0f);
	UserInterface::getSpriteBatch()->End();
}

UIPlaying::UIPlaying() : shouldInit(true), respawnTimer(0), timerScale(0.5f)
{
}

UIPlaying::~UIPlaying()
{
}

void UIPlaying::init()
{
	this->healthBar = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, 20));
	this->minimap = std::make_unique<Minimap>(Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
	this->objectiveBox = std::make_unique<ObjectiveBox>(Vector2(-10, 15));
	this->timer = std::make_unique<AnimatedText>("00:00", Color(Colors::Yellow), timerScale, Animation::NONE);
	this->timer->setPosition(Vector2(SCREEN_WIDTH / 2 - this->timer->getSize().x / 2, 20 + Slider::size.y + 10));
	this->timer->beginAnimation();
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}

bool UIPlaying::hasExploredOnMinimap(Vector3 worldPosition) const
{
	return this->minimap->hasExplored(worldPosition);
}
