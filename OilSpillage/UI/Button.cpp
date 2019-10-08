#include "Button.h"
#include "UserInterface.h"
#include "..//game.h"
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
	UserInterface::getSpriteBatch()->Draw(this->texture->getShaderResView(), this->position, nullptr, selected ? Colors::Gray : Colors::White);

	Vector2 textSize(UserInterface::getFontArial()->MeasureString(this->text.text.c_str()) * 0.4f);
	Vector2 pos(this->position);

	if (this->text.alignment == TopMiddle || this->text.alignment == Center || this->text.alignment == BottomMiddle)
	{
		pos.x += Button::size.x / 2 - textSize.x / 2;
	}
	else if (this->text.alignment == TopRight || this->text.alignment == MiddleRight || this->text.alignment == BottomRight)
	{
		pos.x += Button::size.x - textSize.x;
	}

	if (this->text.alignment == MiddleLeft || this->text.alignment == Center || this->text.alignment == MiddleRight)
	{
		pos.y += Button::size.y / 2 - textSize.y / 2;
	}
	else if (this->text.alignment == BottomLeft || this->text.alignment == BottomMiddle || this->text.alignment == BottomRight)
	{
		pos.y += Button::size.y - textSize.y;
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.text.c_str(), pos, this->text.colour, 0.0f, Vector2(), 0.4f);
}
