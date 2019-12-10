#include "UserInterface.h"
#include "..//game.h"
#include "..//Input.h"

std::unique_ptr<CommonStates> UserInterface::commonStates;
std::unique_ptr<SpriteBatch> UserInterface::spriteBatch;
std::unique_ptr<SpriteFont> UserInterface::fontArial;

CommonStates* UserInterface::getCommonStates()
{
	return UserInterface::commonStates.get();
}

SpriteBatch* UserInterface::getSpriteBatch()
{
	return UserInterface::spriteBatch.get();
}

SpriteFont* UserInterface::getFontArial()
{
	return UserInterface::fontArial.get();
}

void UserInterface::initStaticVariables()
{
	UserInterface::commonStates = std::make_unique<CommonStates>(Game::getGraphics().getDevice());
	UserInterface::spriteBatch = std::make_unique<SpriteBatch>(Game::getGraphics().getDeviceContext());
	UserInterface::fontArial = std::make_unique<SpriteFont>(Game::getGraphics().getDevice(), L"data/bebas.spritefont");
}

Element* UserInterface::changeSelected(Element* selected)
{
	if (Input::checkButton(Keys::L_DOWN, States::PRESSED))
	{
		if (selected->getDown() != nullptr)
		{
			selected = selected->getDown();
			Sound::play("MenuMove.wav");
		}
	}

	if (Input::checkButton(Keys::L_UP, States::PRESSED))
	{
		if (selected->getUp() != nullptr)
		{
			selected = selected->getUp();
			Sound::play("MenuMove.wav");
		}
	}

	if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
	{
		if (selected->getLeft() != nullptr)
		{
			selected = selected->getLeft();
			Sound::play("MenuMove.wav");
		}
	}

	if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
	{
		if (selected->getRight() != nullptr)
		{
			selected = selected->getRight();
			Sound::play("MenuMove.wav");
		}
	}

	return selected;
}

void UserInterface::resetShaders() const
{
	ID3D11DeviceContext* deviceContext = Game::getGraphics().getDeviceContext();
	deviceContext->VSSetShader(nullptr, nullptr, 0);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);
	deviceContext->IASetInputLayout(nullptr);
}

UserInterface::UserInterface()
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::update(float deltaTime)
{
	this->updateUI(deltaTime);

	//this->resetShaders();
	this->drawUI();
}

void UserInterface::unloadTextures()
{
}
