#include "UICompletedStage.h"
#include "../../game.h"

void UICompletedStage::updateUI(float deltaTime)
{
	this->selected = UserInterface::changeSelected(this->selected);
	this->textResults->update(deltaTime);

	for (int i = 0; i < 8; i++)
	{
		this->textStats[i]->update(deltaTime);
	}
}

void UICompletedStage::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->textResults->draw(false);

	for (int i = 0; i < 8; i++)
	{
		this->textStats[i]->draw(false);
	}
	
	this->buttonSaveContinue->draw(this->selected == this->buttonSaveContinue.get());
	this->buttonSaveExit->draw(this->selected == this->buttonSaveExit.get());
	UserInterface::getSpriteBatch()->End();
}

UICompletedStage::UICompletedStage() : selected(nullptr)
{
}

UICompletedStage::~UICompletedStage()
{
}

void UICompletedStage::init()
{
	const GameInfo& info = Game::getGameInfo();

	this->textResults = std::make_unique<AnimatedText>("Results:", Color(Colors::White), 1.0f, Animation::SPINNING);
	this->textResults->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textResults->getSize().x, 10));
	this->textResults->beginAnimation();

	this->textStats[0] = std::make_unique<AnimatedText>("Cleared Stages: " + std::to_string(info.nrOfClearedStages), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[0]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[0]->getSize().x, 10));
	this->textStats[0]->beginAnimation();
	this->textStats[1] = std::make_unique<AnimatedText>("Cleared Objectives: " + std::to_string(info.nrOfObjectives), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[1]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[1]->getSize().x, 10));
	this->textStats[1]->beginAnimation();
	this->textStats[2] = std::make_unique<AnimatedText>("Swarms Killed: " + std::to_string(info.nrOfSwarm), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[2]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[2]->getSize().x, 10));
	this->textStats[2]->beginAnimation();
	this->textStats[3] = std::make_unique<AnimatedText>("Attackers Killed: " + std::to_string(info.nrOfAttackers), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[3]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[3]->getSize().x, 10));
	this->textStats[3]->beginAnimation();
	this->textStats[4] = std::make_unique<AnimatedText>("Cars Destroyed: " + std::to_string(info.nrOfCars), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[4]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[4]->getSize().x, 10));
	this->textStats[4]->beginAnimation();
	this->textStats[5] = std::make_unique<AnimatedText>("Bosses Eliminated: " + std::to_string(info.nrOfBosses), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[5]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[5]->getSize().x, 10));
	this->textStats[5]->beginAnimation();
	this->textStats[6] = std::make_unique<AnimatedText>("Stage Score: " + std::to_string(info.highScore), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[6]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[6]->getSize().x, 10));
	this->textStats[6]->beginAnimation();
	this->textStats[7] = std::make_unique<AnimatedText>("TOTAL SCORE: " + std::to_string(info.highScore), Color(Colors::LightGreen), 0.75f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[7]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[7]->getSize().x, 10));
	this->textStats[7]->beginAnimation();

	this->buttonSaveContinue = std::make_unique<Button>(Text("Save and continue", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x - 10, SCREEN_HEIGHT - Button::size.y - 8 - 32 - 8));
	this->buttonSaveExit = std::make_unique<Button>(Text("Save and exit", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 + 10, SCREEN_HEIGHT - Button::size.y - 8 - 32 - 8));

	this->buttonSaveContinue->setNeighbours(this->buttonSaveExit.get(), this->buttonSaveExit.get(), nullptr, nullptr);
	this->buttonSaveExit->setNeighbours(this->buttonSaveContinue.get(), this->buttonSaveContinue.get(), nullptr, nullptr);
	this->selected = this->buttonSaveContinue.get();
}
