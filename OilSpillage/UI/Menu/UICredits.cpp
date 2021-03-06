#include "UICredits.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include <fstream>

void UICredits::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::CONFIRM, States::PRESSED) || Input::checkButton(Keys::CANCEL, States::PRESSED))
	{
		static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_MAIN);
	}
}

void UICredits::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	
	for (size_t i = 0; i < this->creditsText.size(); i++)
	{
		Vector2 textSize(UserInterface::getFontArial()->MeasureString(this->creditsText[i].c_str()) * 0.4f);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->creditsText[i].c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 5 + 80 * 0.4f * i), Colors::Yellow, 0.0f, Vector2(), 0.4f);
	}

	this->buttonBack->draw(true);
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UICredits::UICredits()
{
}

UICredits::~UICredits()
{
}

void UICredits::init()
{
	std::string temp;
	std::ifstream creditsFile("data/credits.txt");

	this->creditsText.clear();

	while (!creditsFile.eof())
	{
		std::getline(creditsFile, temp);
		this->creditsText.push_back(temp);
	}

	creditsFile.close();

	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH - Button::size.x - 10, SCREEN_HEIGHT - Button::size.y - 10));

	Prompt prompts[] = {
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(10, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}

void UICredits::unloadTextures()
{
	this->buttonBack->unloadTextures();
	this->promptBar->unloadTextures();
}
