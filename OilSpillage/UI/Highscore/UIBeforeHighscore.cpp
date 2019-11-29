#include "UIBeforeHighscore.h"
#include "../../game.h"
#include "../../Input.h"
#include <cassert>
#include "../../States/HighscoreGameState.h"

void UIBeforeHighscore::updateUI(float deltaTime)
{
	if (Input::isKeyPressed(Keyboard::Keys::A))
	{
		this->name += "a";
	}
	else if (Input::isKeyPressed(Keyboard::Keys::B))
	{
		this->name += "b";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::C))
	{
		this->name += "c";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::D))
	{
		this->name += "d";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::E))
	{
		this->name += "e";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::F))
	{
		this->name += "f";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::G))
	{
		this->name += "g";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::H))
	{
		this->name += "h";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::I))
	{
		this->name += "i";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::J))
	{
		this->name += "j";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::K))
	{
		this->name += "k";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::L))
	{
		this->name += "l";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::M))
	{
		this->name += "m";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::N))
	{
		this->name += "n";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::O))
	{
		this->name += "o";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::P))
	{
		this->name += "p";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::Q))
	{
		this->name += "q";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::R))
	{
		this->name += "r";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::S))
	{
		this->name += "s";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::T))
	{
		this->name += "t";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::U))
	{
		this->name += "u";
	}
	else if (Input::isKeyPressed(Keyboard::Keys::V))
	{
		this->name += "v";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::W))
	{
		this->name += "w";
	}
	else if (Input::isKeyPressed(Keyboard::Keys::X))
	{
		this->name += "x";
	}
	else if(Input::isKeyPressed(Keyboard::Keys::Y))
	{
		this->name += "y";
	}
	else if (Input::isKeyPressed(Keyboard::Keys::Z))
	{
		this->name += "z";
	}
	else if (Input::isKeyPressed(Keyboard::Keys::Back) && this->name.size() > 0)
	{
		this->name.resize(this->name.size() - 1);
	}

	this->theName->setVariables(this->name, {}, {});
	this->theName->setPosition(Vector2(SCREEN_WIDTH / 2 - this->theName->getSize().x / 2, SCREEN_HEIGHT / 2 - this->theName->getSize().y / 2 + this->yourName->getSize().y));
	this->theName->update(deltaTime);
	this->yourName->update(deltaTime);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		static_cast<HighscoreGameState*>(Game::getCurrentState())->setCurrentMenu(HighscoreGameState::MENU_HIGHSCORE);
	}
	else if (Input::checkButton(Keys::MENU, States::PRESSED))
	{
		Game::setState(Game::STATE_MENU);
	}
}

void UIBeforeHighscore::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->yourName->draw(false);
	this->theName->draw(false);
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIBeforeHighscore::UIBeforeHighscore()
{
}

UIBeforeHighscore::~UIBeforeHighscore()
{
}

void UIBeforeHighscore::init()
{
	this->yourName = std::make_unique<AnimatedText>("Your name:", Color(Colors::White), 0.5f, Animation::SHAKING_FADE_IN_STOP);
	this->yourName->setPosition(Vector2(SCREEN_WIDTH / 2 - this->yourName->getSize().x / 2, SCREEN_HEIGHT / 2 - this->yourName->getSize().y / 2 - 10));
	this->yourName->beginAnimation();

	this->theName = std::make_unique<AnimatedText>(this->name, Color(Colors::White), 0.5f, Animation::NONE);
	this->theName->setPosition(Vector2(SCREEN_WIDTH / 2 - this->theName->getSize().x / 2, SCREEN_HEIGHT / 2 - this->theName->getSize().y / 2 + this->yourName->getSize().y));
	this->theName->beginAnimation();
	
	Prompt prompts[] = {
		{ Keys::CONFIRM, "Submit Highscore", Color(Colors::White) },
		{ Keys::MENU, "Return To Menu", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}

std::string UIBeforeHighscore::getName() const
{
	return this->name;
}
