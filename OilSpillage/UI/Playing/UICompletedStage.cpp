#include "UICompletedStage.h"
#include "../../game.h"

void UICompletedStage::updateUI(float deltaTime)
{
	this->selected = UserInterface::changeSelected(this->selected);
	this->textResults->update(deltaTime);

	if (timer < 0.25f * 8)
	{
		this->timer += deltaTime;
	}

	for (int i = 0; i < 8; i++)
	{
		if (this->timer > 0.25f * i) this->textStats[i]->update(deltaTime);
	}

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonSaveContinue.get())
		{
			Game::setState(Game::STATE_UPGRADING);
		}
		else
		{
			Game::setState(Game::STATE_MENU);
		}
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
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UICompletedStage::UICompletedStage() : selected(nullptr), timer(0.0f)
{
}

UICompletedStage::~UICompletedStage()
{
}

void UICompletedStage::init()
{
	const GameInfo& info = Game::getGameInfo();

	this->textResults = std::make_unique<AnimatedText>("Results:", Color(Colors::White), 1.0f, Animation::SPINNING);
	this->textResults->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textResults->getSize().x / 2, 50));
	this->textResults->beginAnimation();

	float offset = 50.0f;
	this->textStats[0] = std::make_unique<AnimatedText>("Cleared Stages: " + std::to_string(info.nrOfClearedStages), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[0]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[0]->getSize().x / 2, 170 + offset * 0));
	this->textStats[0]->beginAnimation();
	this->textStats[1] = std::make_unique<AnimatedText>("Cleared Objectives: " + std::to_string(info.nrOfObjectives), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[1]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[1]->getSize().x / 2, 170 + offset * 1));
	this->textStats[1]->beginAnimation();
	this->textStats[2] = std::make_unique<AnimatedText>("Swarms Killed: " + std::to_string(info.nrOfSwarm), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[2]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[2]->getSize().x / 2, 170 + offset * 2));
	this->textStats[2]->beginAnimation();
	this->textStats[3] = std::make_unique<AnimatedText>("Attackers Killed: " + std::to_string(info.nrOfAttackers), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[3]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[3]->getSize().x / 2, 170 + offset * 3));
	this->textStats[3]->beginAnimation();
	this->textStats[4] = std::make_unique<AnimatedText>("Cars Destroyed: " + std::to_string(info.nrOfCars), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[4]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[4]->getSize().x / 2, 170 + offset * 4));
	this->textStats[4]->beginAnimation();
	this->textStats[5] = std::make_unique<AnimatedText>("Bosses Eliminated: " + std::to_string(info.nrOfBosses), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[5]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[5]->getSize().x / 2, 170 + offset * 5));
	this->textStats[5]->beginAnimation();
	this->textStats[6] = std::make_unique<AnimatedText>("Stage Score: " + std::to_string(info.highScoreStage), Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[6]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[6]->getSize().x / 2, 170 + offset * 6));
	this->textStats[6]->beginAnimation();
	this->textStats[7] = std::make_unique<AnimatedText>("TOTAL SCORE: " + std::to_string(info.highScoreTotal), Color(Colors::LightGreen), 0.75f, Animation::SHAKING_FADE_IN_STOP);
	this->textStats[7]->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textStats[7]->getSize().x / 2, 170 + offset * 7));
	this->textStats[7]->beginAnimation();

	this->buttonSaveContinue = std::make_unique<Button>(Text("Save & continue", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x - 10, SCREEN_HEIGHT - Button::size.y - 8 - 32 - 8));
	this->buttonSaveExit = std::make_unique<Button>(Text("Save & exit", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 + 10, SCREEN_HEIGHT - Button::size.y - 8 - 32 - 8));

	this->buttonSaveContinue->setNeighbours(this->buttonSaveExit.get(), this->buttonSaveExit.get(), nullptr, nullptr);
	this->buttonSaveExit->setNeighbours(this->buttonSaveContinue.get(), this->buttonSaveContinue.get(), nullptr, nullptr);
	this->selected = this->buttonSaveContinue.get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
