#include "Slider.h"
#include "UserInterface.h"
#include "..//game.h"

Vector2 Slider::size = Vector2(128, 32);

Slider::Slider(Vector2 position) : Element(position, 0.0f), amount(0.0f)
{
	Game::getGraphics().loadTexture("sliderBG.tga");
	Game::getGraphics().loadTexture("sliderFG.tga");
	this->textureBG = Game::getGraphics().getTexturePointer("sliderBG.tga");
	this->textureFG = Game::getGraphics().getTexturePointer("sliderFG.tga");
}

Slider::~Slider()
{
}

void Slider::draw(bool selected)
{
	RECT bgDest = SimpleMath::Rectangle(this->position.x + 8, this->position.y + 8, this->textureFG->getWidth() - 16, this->textureBG->getHeight());
	RECT amountDest = SimpleMath::Rectangle(this->position.x + 8, this->position.y + 8, (this->textureFG->getWidth() - 16) * this->amount, this->textureBG->getHeight());

	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), bgDest, selected ? Colors::Gray : Colors::White);
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
