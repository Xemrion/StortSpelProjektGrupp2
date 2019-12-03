#include "Button.h"
#include "../UserInterface.h"
#include "../../game.h"
#include <cassert>

Vector2 Button::size = Vector2(256, 64);

Button::Button(Text text, Vector2 position) : Element(position, 0.0f), text(text)
{
	Game::getGraphics().loadTexture("UI/button");
	this->texture = Game::getGraphics().getTexturePointer("UI/button");
   assert( texture && "Texture failed to load!" );
}

Button::~Button()
{
}

void Button::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->texture->getShaderResView(), this->position, nullptr, selected ? Colors::White : Colors::Gray);

	float scale = 0.4f;
	Vector2 textSize(UserInterface::getFontArial()->MeasureString(this->text.text.c_str()) * scale);
	if (textSize.x > Button::size.x - 16)
	{
		scale *= (Button::size.x - 16) / textSize.x;
		textSize *= (Button::size.x - 16) / textSize.x;
	}
	
	if (textSize.y > Button::size.y - 16)
	{
		scale *= (Button::size.y - 16) / textSize.y;
		textSize *= (Button::size.y - 16) / textSize.y;
	}

	Vector2 pos(this->position);

	if (this->text.alignment == TextAlignment::TopMiddle || this->text.alignment == TextAlignment::Center || this->text.alignment == TextAlignment::BottomMiddle)
	{
		pos.x += Button::size.x / 2 - textSize.x / 2;
	}
	else if (this->text.alignment == TextAlignment::TopRight || this->text.alignment == TextAlignment::MiddleRight || this->text.alignment == TextAlignment::BottomRight)
	{
		pos.x += Button::size.x - textSize.x;
	}

	if (this->text.alignment == TextAlignment::MiddleLeft || this->text.alignment == TextAlignment::Center || this->text.alignment == TextAlignment::MiddleRight)
	{
		pos.y += Button::size.y / 2 - textSize.y / 2;
	}
	else if (this->text.alignment == TextAlignment::BottomLeft || this->text.alignment == TextAlignment::BottomMiddle || this->text.alignment == TextAlignment::BottomRight)
	{
		pos.y += Button::size.y - textSize.y;
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.text.c_str(), pos, this->text.colour, 0.0f, Vector2(), scale);
}

void Button::setText(std::string text)
{
	this->text.text = text;
}
