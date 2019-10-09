#include "Slider.h"
#include "UserInterface.h"
#include "..//game.h"
#include <cassert>

Vector2 Slider::size = Vector2(128, 32);

Slider::Slider(Vector2 position) : Element(position, 0.0f), amount(0.0f)
{
	Game::getGraphics().loadTexture("UI/sliderBG");
	Game::getGraphics().loadTexture("UI/sliderFG");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/sliderBG");
	this->textureFG = Game::getGraphics().getTexturePointer("UI/sliderFG");
   assert( textureBG && "Texture failed to load!" );
   assert( textureFG && "Texture failed to load!" );
}

Slider::~Slider()
{
}

void Slider::draw(bool selected)
{
	RECT bgDest = SimpleMath::Rectangle(static_cast<long>(this->position.x + 8), static_cast<long>(this->position.y + 8), static_cast<long>(this->textureFG->getWidth() - 16), static_cast<long>(this->textureBG->getHeight()));
	RECT amountDest = SimpleMath::Rectangle(static_cast<long>(this->position.x + 8), static_cast<long>(this->position.y + 8), static_cast<long>((this->textureFG->getWidth() - 16) * this->amount), static_cast<long>(this->textureBG->getHeight()));

	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), bgDest,     selected ? Colors::Gray : Colors::White);
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), amountDest, selected ? Colors::DarkGreen : Colors::Green);
	UserInterface::getSpriteBatch()->Draw(this->textureFG->getShaderResView(), this->position);
}

void Slider::setAmount(float amount)
{
	this->amount = std::clamp(amount, 0.0f, 1.0f);
}

float Slider::getAmount() const
{
	return this->amount;
}
