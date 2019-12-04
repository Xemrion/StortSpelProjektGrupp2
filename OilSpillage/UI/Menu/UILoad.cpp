#include "UILoad.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include "../../SaveSystem.h"

void UILoad::updateUI(float deltaTime)
{
	this->selected = UserInterface::changeSelected(this->selected);
	this->textFileSelect->update(deltaTime);
	bool oldErasing = this->erasing;

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonErase.get())
		{
			this->erasing = !this->erasing;
		}
		else if (this->selected == this->buttonBack.get())
		{
			this->erasing = false;
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_MAIN);
		}
		else
		{
			for (int i = 0; i < 5; i++)
			{
				if (this->selected == this->buttonFile[i].get())
				{
					if (this->erasing)
					{
						remove(("file" + std::to_string(i) + ".sav").c_str());
						this->buttonFile[i]->setText(SaveSystem::fileInfo(i));
						this->erasing = false;
					}
					else
					{
						SaveSystem::loadGame(i, static_cast<MenuGameState*>(Game::getCurrentState())->getSlots());
						static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_CONTROLS);
					}
				}
			}
		}
	}
	else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
	{
		if (this->erasing)
		{
			this->erasing = false;
		}
		else
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_MAIN);
		}
	}

	if (oldErasing != this->erasing)
	{
		this->textFileSelect->setVariables(this->erasing ? "Erasing File:" : "Select File:", this->erasing ? Color(Colors::Red) : Color(Colors::White), {});
		this->textFileSelect->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textFileSelect->getSize().x / 2, 10));
	}
}

void UILoad::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->textFileSelect->draw(false);

	for (int i = 0; i < 5; i++)
	{
		this->buttonFile[i]->draw(this->selected == this->buttonFile[i].get());
	}

	this->buttonErase->draw(this->selected == this->buttonErase.get());
	this->buttonBack->draw(this->selected == this->buttonBack.get());
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UILoad::UILoad() : selected(nullptr), erasing(false)
{
}

UILoad::~UILoad()
{
}

void UILoad::init()
{
	this->textFileSelect = std::make_unique<AnimatedText>("Select File:", Color(Colors::White), 0.75f, Animation::NONE);
	this->textFileSelect->setPosition(Vector2(SCREEN_WIDTH / 2 - this->textFileSelect->getSize().x / 2, 10));
	this->textFileSelect->beginAnimation();

	for (int i = 0; i < 5; i++)
	{
		this->buttonFile[i] = std::make_unique<Button>(Text(SaveSystem::fileInfo(i), Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - 100 + Button::size.y * ((i - 2) + 0.25f * (i - 2))));
	}

	this->buttonErase = std::make_unique<Button>(Text("Erase", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT - 48 - Button::size.y * 2.25f));
	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT - 48 - Button::size.y));

	this->buttonFile[0]->setNeighbours(nullptr, nullptr, this->buttonBack.get(),    this->buttonFile[1].get());
	this->buttonFile[1]->setNeighbours(nullptr, nullptr, this->buttonFile[0].get(), this->buttonFile[2].get());
	this->buttonFile[2]->setNeighbours(nullptr, nullptr, this->buttonFile[1].get(), this->buttonFile[3].get());
	this->buttonFile[3]->setNeighbours(nullptr, nullptr, this->buttonFile[2].get(), this->buttonFile[4].get());
	this->buttonFile[4]->setNeighbours(nullptr, nullptr, this->buttonFile[3].get(), this->buttonErase.get());
	this->buttonErase->setNeighbours(  nullptr, nullptr, this->buttonFile[4].get(), this->buttonBack.get());
	this->buttonBack->setNeighbours(   nullptr, nullptr, this->buttonErase.get(),   this->buttonFile[0].get());
	this->selected = this->buttonFile[0].get();

	Prompt prompts[] = {
		   { Keys::L_PRESS, "Move", Color(Colors::White) },
		   { Keys::CONFIRM, "Confirm", Color(Colors::White) },
		   { Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 3);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
