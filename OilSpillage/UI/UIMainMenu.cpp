#include "UIMainMenu.h"
#include "..//Input.h"
#include "..//game.h"
#include "..//States/MenuGameState.h"

void UIMainMenu::updateUI(float deltaTime)
{
	this->selected = this->changeSelected(this->selected);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonPlay.get())
		{
			Game::setState(Game::STATE_PLAYING);
		}
		else if (this->selected == this->buttonOptions.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_OPTIONS);
		}
		else if (this->selected == this->buttonCredits.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_CREDITS);
		}
		else if (this->selected == this->buttonQuit.get())
		{
			Game::quit();
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
}

void UIMainMenu::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->buttonPlay->draw(this->selected == this->buttonPlay.get());
	this->buttonOptions->draw(this->selected == this->buttonOptions.get());
	this->buttonCredits->draw(this->selected == this->buttonCredits.get());
	this->buttonQuit->draw(this->selected == this->buttonQuit.get());
	this->checkBoxTest->draw(this->selected == this->checkBoxTest.get());
	this->sliderTest->draw(this->selected == this->sliderTest.get());
	UserInterface::getSpriteBatch()->End();
}

UIMainMenu::UIMainMenu() : selected(nullptr)
{
}

UIMainMenu::~UIMainMenu()
{
}

void UIMainMenu::init()
{
	this->buttonPlay = std::make_unique<Button>(Text("Play", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->buttonOptions = std::make_unique<Button>(Text("Options", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.25f));
	this->buttonCredits = std::make_unique<Button>(Text("Credits", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 0.25f));
	this->buttonQuit = std::make_unique<Button>(Text("Quit", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 1.75f));
	this->checkBoxTest = std::make_unique<CheckBox>(Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2 - 10 - CheckBox::size.x, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->sliderTest = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 3.25f));

	this->buttonPlay->setNeighbours(this->checkBoxTest.get(), this->checkBoxTest.get(), this->sliderTest.get(), this->buttonOptions.get());
	this->buttonOptions->setNeighbours(nullptr, nullptr, this->buttonPlay.get(), this->buttonCredits.get());
	this->buttonCredits->setNeighbours(nullptr, nullptr, this->buttonOptions.get(), this->buttonQuit.get());
	this->buttonQuit->setNeighbours(nullptr, nullptr, this->buttonCredits.get(), this->sliderTest.get());
	this->checkBoxTest->setNeighbours(this->buttonPlay.get(), this->buttonPlay.get(), nullptr, nullptr);
	this->sliderTest->setNeighbours(nullptr, nullptr, this->buttonQuit.get(), this->buttonPlay.get());

	this->sliderTest->setAmount(0.5f);

	this->selected = this->buttonPlay.get();
}
