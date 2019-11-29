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
	int time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	Color color(Colors::Yellow);
	Vector2 position = this->timer->getPosition();

	if (this->shouldInit)
	{
		this->minimap->init();
		this->objectiveBox->init();
		this->lastMinute = static_cast<int>((time - 1.0f) / 60);
		this->shouldInit = false;
	}

	this->respawnTimer += deltaTime;
	this->minimap->update(deltaTime);
	this->objectiveBox->update(deltaTime);

	if (this->timeChangeText.get())
	{
		this->timeChangeTimer += deltaTime;
		if (this->timeChangeTimer <= 1.0f)
		{
			this->timeChangeText->setPosition(this->timer->getPosition() + this->timer->getSize() * 0.5f - this->timeChangeText->getSize() * 0.5f + Vector2(0, this->timeChangeTimer * 20.0f));
		}
		else
		{
			this->timeChangeText.reset();
		}
	}

	int thisMinute = static_cast<int>((time - 1.0f) / 60);
	if (thisMinute < this->lastMinute)
	{
		this->scaleUp = true;
		this->scaleTimer = 0.0f;
	}
	this->lastMinute = thisMinute;

	if ((this->scaleUp && this->scaleTimer < 3.0f) || time <= 10.0f)
	{
		this->scaleTimer += deltaTime;

		timerScale = Game::lerp(timerScale, 1.0f, deltaTime * 2);
		position = Vector2::Lerp(position, Vector2(SCREEN_WIDTH / 2 - 100, 20 + Slider::size.y + 10), deltaTime * 2);
		color = Color(Colors::GreenYellow);
	}
	else
	{
		timerScale = Game::lerp(timerScale, 0.5f, deltaTime * 2);
		position = Vector2::Lerp(position, Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2 - 115, 10), deltaTime * 2);
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
	this->timer->setPosition(position);
	this->timer->update(deltaTime);

	this->score->setVariables("Score: " + std::to_string(Game::getGameInfo().highScore), {}, {});
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
	Objective* currObj = static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler().getObjective(0);
	if (currObj != nullptr && currObj->getType() == TypeOfMission::BossEvent)
	{
		this->bossHealthBar->setAmount(currObj->getBoss()->getHealth() / static_cast<float>(currObj->getBoss()->getMaxHealth()));
		this->bossShieldBar->setAmount((currObj->getBoss()->getTotalWeakSpotCurrHp())
		/ (static_cast<float>(currObj->getBoss()->getTotalWeakSpotMaxHP())));
	}

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
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Driving mode:", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(500, -340.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
	if (!Game::getDrivingMode()) {
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Arcade", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(500, -320.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
	}else {
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Realistic", Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) + Vector2(500, -320.0f), Colors::White, 0, Vector2(0.2f, 0.2f), 0.2f);
	}

	if (currObj != nullptr && currObj->getType() == TypeOfMission::BossEvent)
	{
		if ((currObj->getBoss()->getTotalWeakSpotCurrHp()) > 0) //draw sield only if exists
		{
			Vector2 textBoss = UserInterface::getFontArial()->MeasureString("Qwerty SHIELDS");
			this->bossShieldBar->draw(false);
			UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Qwerty SHIELDS", Vector2((SCREEN_WIDTH / 2), 590), Colors::Yellow, 0, Vector2(textBoss.x / 2, textBoss.y / 2), 0.4f);
		}

		Vector2 textBoss = UserInterface::getFontArial()->MeasureString("Qwerty");
		this->bossHealthBar->draw(false); //false = change color to gray
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Qwerty", Vector2((SCREEN_WIDTH / 2) , 660), Colors::Yellow, 0, Vector2(textBoss.x / 2, textBoss.y / 2), 0.4f);
	}
	this->healthBar->draw(false);
	this->objectiveBox->draw(false);
	this->minimap->draw(false);
	this->timerText->draw(false);
	this->timer->draw(false);
	if (this->timeChangeText.get()) this->timeChangeText->draw(false);
	this->score->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIPlaying::UIPlaying() : shouldInit(true), respawnTimer(0), timerScale(0.5f), lastMinute(0), scaleUp(false), scaleTimer(0.0f)
{
}

UIPlaying::~UIPlaying()
{
}

void UIPlaying::init()
{
	this->healthBar = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, 20));
	this->bossHealthBar = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, 680), Colors::Red);
	this->bossShieldBar = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, 610), Colors::DarkGray);

	this->minimap = std::make_unique<Minimap>(Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
	this->objectiveBox = std::make_unique<ObjectiveBox>(Vector2(10, 10));

	this->timerText = std::make_unique<AnimatedText>("Time: ", Color(Colors::Yellow), 0.5f, Animation::NONE, Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2 - 115 - 100, 10));
	this->timerText->beginAnimation();
	this->timer = std::make_unique<AnimatedText>("00:00", Color(Colors::Yellow), timerScale, Animation::NONE, Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2 - 115, 10));
	this->timer->beginAnimation();
	this->score = std::make_unique<AnimatedText>("Score: 0", Color(Colors::Yellow), 0.5f, Animation::NONE, Vector2(SCREEN_WIDTH / 2 + Slider::size.x / 2 + 10, 10));
	this->score->beginAnimation();
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}

bool UIPlaying::hasExploredOnMinimap(Vector3 worldPosition) const
{
	return this->minimap->hasExplored(worldPosition);
}

void UIPlaying::addTimeChangeText(float amount)
{
	this->timeChangeText = std::make_unique<AnimatedText>((amount < 0 ? "" : "+") + std::to_string(static_cast<int>(amount)), amount < 0 ? Color(Colors::Red) : Color(Colors::Green), 0.5f, Animation::NONE);
	this->timeChangeText->setPosition(this->timer->getPosition() + this->timer->getSize() * 0.5f - this->timeChangeText->getSize() * 0.5f);
	this->timeChangeText->beginAnimation();

	this->timeChangeTimer = 0.0f;
}
