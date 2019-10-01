#include "UserInterface.h"
#include "..//game.h"

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

void UserInterface::init()
{
	UserInterface::commonStates = std::make_unique<CommonStates>(Game::getGraphics().getDevice());
	UserInterface::spriteBatch = std::make_unique<SpriteBatch>(Game::getGraphics().getDeviceContext());
	UserInterface::fontArial = std::make_unique<SpriteFont>(Game::getGraphics().getDevice(), L"arial.spritefont");
}

void UserInterface::resetShaders()
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
