#include "UIMainMenu.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"

void UIMainMenu::updateUI(float deltaTime)
{
	this->selected = this->changeSelected(this->selected);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonPlay.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_LOAD);
		}
		else if (this->selected == this->buttonOptions.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_OPTIONS);
		}
		else if (this->selected == this->buttonCredits.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_CREDITS);
		}
		else if (this->selected == this->buttonHighscore.get())
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_HIGHSCORE);
		}
		else if (this->selected == this->buttonQuit.get())
		{
			Game::quit();
		}
	}
}

void UIMainMenu::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->buttonPlay->draw(this->selected == this->buttonPlay.get());
	this->buttonOptions->draw(this->selected == this->buttonOptions.get());
	this->buttonCredits->draw(this->selected == this->buttonCredits.get());
	this->buttonHighscore->draw(this->selected == this->buttonHighscore.get());
	this->buttonQuit->draw(this->selected == this->buttonQuit.get());
	this->promptBar->draw(false);
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
	this->buttonHighscore = std::make_unique<Button>(Text("Highscore", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 1.75f));
	this->buttonQuit = std::make_unique<Button>(Text("Quit", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 + Button::size.y * 3.25f));
	
	this->buttonPlay->setNeighbours(nullptr, nullptr, this->buttonQuit.get(), this->buttonOptions.get());
	this->buttonOptions->setNeighbours(nullptr, nullptr, this->buttonPlay.get(), this->buttonCredits.get());
	this->buttonCredits->setNeighbours(nullptr, nullptr, this->buttonOptions.get(), this->buttonHighscore.get());
	this->buttonHighscore->setNeighbours(nullptr, nullptr, this->buttonCredits.get(), this->buttonQuit.get());
	this->buttonQuit->setNeighbours(nullptr, nullptr, this->buttonHighscore.get(), this->buttonPlay.get());

	this->selected = this->buttonPlay.get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}

void UIMainMenu::unloadTextures()
{
	this->promptBar->unloadTextures();
	this->buttonCredits->unloadTextures();//has the same textures so only one button needs to be unloaded
}
