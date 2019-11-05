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
		else if (this->selected == this->checkBoxTest.get())
		{
			this->checkBoxTest->setActivated(!this->checkBoxTest->isActivated());
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
	this->checkBoxTest->draw(this->selected == this->checkBoxTest.get());
	this->sliderTest->draw(this->selected == this->sliderTest.get());
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
	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->checkBoxTest = std::make_unique<CheckBox>(Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2 - 10 - CheckBox::size.x, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->sliderTest = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.25f));

	this->buttonBack->setNeighbours(this->checkBoxTest.get(), this->checkBoxTest.get(), this->sliderTest.get(), this->sliderTest.get());
	this->checkBoxTest->setNeighbours(this->buttonBack.get(), this->buttonBack.get(), nullptr, nullptr);
	this->sliderTest->setNeighbours(nullptr, nullptr, this->buttonBack.get(), this->buttonBack.get());

	this->sliderTest->setAmount(0.5f);

	this->selected = this->buttonBack.get();
}
