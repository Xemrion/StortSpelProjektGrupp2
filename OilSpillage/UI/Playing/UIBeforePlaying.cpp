#include "UIBeforePlaying.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"
#include "../../PG/Environment.hpp"

void UIBeforePlaying::updateUI(float deltaTime)
{
	this->textStage->update(deltaTime);
	this->textName->update(deltaTime);
	this->textSize->update(deltaTime);
	this->textBiome->update(deltaTime);
	this->textTime->update(deltaTime);
	this->textWeather->update(deltaTime);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		static_cast<PlayingGameState*>(Game::getCurrentState())->setCurrentMenu(PlayingGameState::MENU_PLAYING);
		Game::getGraphics().enableFog();
	}
}

void UIBeforePlaying::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), Vector2::Zero);
	this->textStage->draw(false);
	this->textName->draw(false);
	this->textSize->draw(false);
	this->textBiome->draw(false);
	this->textTime->draw(false);
	this->textWeather->draw(false);
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIBeforePlaying::UIBeforePlaying()
{
	Game::getGraphics().loadTexture("UI/menuBG");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/menuBG");
	assert(textureBG && "Texture failed to load!");
}

UIBeforePlaying::~UIBeforePlaying()
{
	Game::getGraphics().unloadTexture("UI/menuBG");
}

void UIBeforePlaying::init()
{
	int stage = Game::getGameInfo().nrOfClearedStages;
	Map::Info info = static_cast<PlayingGameState*>(Game::getCurrentState())->getMapInfo();

	this->textStage = std::make_unique<AnimatedText>("Act " + std::to_string(((stage - 1) / 3) + 1) + " - Stage " + std::to_string((stage - 1) % 3 + 1), Color(Colors::LightGreen), 1.0f, Animation::FADE_IN);
	this->textStage->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStage->getSize().x / 2, 100));
	this->textStage->beginAnimation();
	this->textName = std::make_unique<AnimatedText>(info.environment.getName(), Color(Colors::YellowGreen), 0.75f, Animation::SHAKING_FADE_IN_STOP);
	this->textName->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textName->getSize().x / 2, 200));
	this->textName->beginAnimation();
	this->textSize = std::make_unique<AnimatedText>("Size: " + std::to_string(info.width) + "x" + std::to_string(info.length), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textSize->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textSize->getLength(6), 300));
	this->textSize->beginAnimation();
	this->textBiome = std::make_unique<AnimatedText>("Biome: " + stringify(info.environment.getBiome()), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textBiome->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textBiome->getLength(7), 350));
	this->textBiome->beginAnimation();
	this->textTime = std::make_unique<AnimatedText>("Time: " + stringify(info.environment.getTime()), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textTime->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textTime->getLength(6), 400));
	this->textTime->beginAnimation();
	this->textWeather = std::make_unique<AnimatedText>("Weather: " + stringify(info.environment.getWeather()), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textWeather->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textWeather->getLength(9), 450));
	this->textWeather->beginAnimation();

	Prompt prompts[] = {
		   { Keys::CONFIRM, "Confirm", Color(Colors::White) },
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 1);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
