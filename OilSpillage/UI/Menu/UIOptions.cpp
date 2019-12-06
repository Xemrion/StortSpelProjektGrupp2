#include "UIOptions.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include "../../States/PlayingGameState.h"

void UIOptions::updateUI(float deltaTime)
{
	this->selected = this->changeSelected(this->selected);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonBack.get())
		{
			MenuGameState* state = dynamic_cast<MenuGameState*>(Game::getCurrentState());

			if (state)
			{
				state->setCurrentMenu(MenuGameState::MENU_MAIN);
			}
			else
			{
				PlayingGameState* state2 = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

				if (state2)
				{
					state2->setCurrentMenu(PlayingGameState::MENU_PAUSED);
				}
			}
		}
		else if (this->selected == this->checkBoxDrivingMode.get())
		{
			this->checkBoxDrivingMode->setActivated(!this->checkBoxDrivingMode->isActivated());
			Game::setDrivingMode(this->checkBoxDrivingMode->isActivated());
		}
		else if (this->selected == this->sliderTest.get())
		{
			if (this->sliderTest->getAmount() == 1.0f)
			{
				this->sliderTest->setAmount(0.0f);
			}
			else
			{
				this->sliderTest->setAmount(this->sliderTest->getAmount() + 0.1f);
			}

			Sound2::setVolumeMaster(this->sliderTest->getAmount());
		}
	}
	else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
	{
		MenuGameState* state = dynamic_cast<MenuGameState*>(Game::getCurrentState());

		if (state)
		{
			state->setCurrentMenu(MenuGameState::MENU_MAIN);
		}
		else
		{
			PlayingGameState* state2 = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

			if (state2)
			{
				state2->setCurrentMenu(PlayingGameState::MENU_PAUSED);
			}
		}
	}
}

void UIOptions::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->buttonBack->draw(this->selected == this->buttonBack.get());
	this->checkBoxDrivingMode->draw(this->selected == this->checkBoxDrivingMode.get());
	this->sliderTest->draw(this->selected == this->sliderTest.get());
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIOptions::UIOptions() : selected(nullptr)
{
}

UIOptions::~UIOptions()
{
}

void UIOptions::init()
{
	this->checkBoxDrivingMode = std::make_unique<CheckBox>("Realistic Driving", Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2 - CheckBox::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->sliderTest = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.25f));
	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 0.25f));

	this->checkBoxDrivingMode->setNeighbours(nullptr, nullptr, this->buttonBack.get(), this->sliderTest.get());
	this->buttonBack->setNeighbours(nullptr, nullptr, this->sliderTest.get(), this->checkBoxDrivingMode.get());
	this->sliderTest->setNeighbours(nullptr, nullptr, this->checkBoxDrivingMode.get(), this->buttonBack.get());

	this->checkBoxDrivingMode->setActivated(Game::getDrivingMode());
	this->sliderTest->setAmount(Sound2::getVolumeMaster());

	this->selected = this->checkBoxDrivingMode.get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 3);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
