#include "UserInterface.h"
#include "..//game.h"

std::unique_ptr<SpriteBatch> UserInterface::spriteBatch;

void UserInterface::initSpriteBatch()
{
	UserInterface::spriteBatch = std::make_unique<SpriteBatch>(Game::getGraphics().getDeviceContext());
}

void UserInterface::setElements(Element** elements, int nrOfElements)
{
	this->elements = elements;
	this->nrOfElements = nrOfElements;
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

SpriteBatch* UserInterface::getSpriteBatch()
{
	return UserInterface::spriteBatch.get();
}

UserInterface::UserInterface()
{
	this->elements = nullptr;
	this->nrOfElements = 0;
}

UserInterface::~UserInterface()
{
}

void UserInterface::update(float deltaTime)
{
	if (this->elements != nullptr)
	{
		for (int i = 0; i < this->nrOfElements; i++)
		{
			this->elements[i]->update(deltaTime);
		}

		this->resetShaders();
		UserInterface::spriteBatch->Begin();

		for (int i = 0; i < this->nrOfElements; i++)
		{
			this->elements[i]->draw();
		}

		UserInterface::spriteBatch->End();
	}
}
