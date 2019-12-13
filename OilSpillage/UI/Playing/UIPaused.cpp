#include "UIPaused.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"

void UIPaused::updateUI(float deltaTime)
{
	this->selected = this->changeSelected(this->selected);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonUnpause.get())
		{
			static_cast<PlayingGameState*>(Game::getCurrentState())->setCurrentMenu(PlayingGameState::MENU_PLAYING);
		}
		else if (this->selected == this->buttonControls.get())
		{
			static_cast<PlayingGameState*>(Game::getCurrentState())->setCurrentMenu(PlayingGameState::MENU_CONTROLS);
		}
		else if (this->selected == this->buttonOptions.get())
		{
			static_cast<PlayingGameState*>(Game::getCurrentState())->setCurrentMenu(PlayingGameState::MENU_OPTIONS);
		}
		else if (this->selected == this->buttonMainMenu.get())
		{
			Game::setState(Game::STATE_MENU);
		}
	}
	else if (Input::checkButton(Keys::CANCEL, States::PRESSED) || Input::checkButton(Keys::MENU, States::PRESSED))
	{
		static_cast<PlayingGameState*>(Game::getCurrentState())->setCurrentMenu(PlayingGameState::MENU_PLAYING);
	}
}

void UIPaused::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->buttonUnpause->draw(this->selected == this->buttonUnpause.get());
	this->buttonControls->draw(this->selected == this->buttonControls.get());
	this->buttonOptions->draw(this->selected == this->buttonOptions.get());
	this->buttonMainMenu->draw(this->selected == this->buttonMainMenu.get());
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIPaused::UIPaused() : selected(nullptr)
{
}

UIPaused::~UIPaused()
{
}

void UIPaused::init()
{
	this->buttonUnpause = std::make_unique<Button>(Text("Resume", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 2));
	this->buttonControls = std::make_unique<Button>(Text("Controls", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y / 2));
	this->buttonOptions = std::make_unique<Button>(Text("Options", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y));
	this->buttonMainMenu = std::make_unique<Button>(Text("Main Menu", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 2.5f));

	this->buttonUnpause->setNeighbours(nullptr, nullptr, this->buttonMainMenu.get(), this->buttonControls.get());
	this->buttonControls->setNeighbours(nullptr, nullptr, this->buttonUnpause.get(), this->buttonOptions.get());
	this->buttonOptions->setNeighbours(nullptr, nullptr, this->buttonControls.get(), this->buttonMainMenu.get());
	this->buttonMainMenu->setNeighbours(nullptr, nullptr, this->buttonOptions.get(), this->buttonUnpause.get());

	this->selected = this->buttonUnpause.get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) },
		{ Keys::CANCEL, "Resume", Color(Colors::White) },
		{ Keys::MENU, "Resume", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 4);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
