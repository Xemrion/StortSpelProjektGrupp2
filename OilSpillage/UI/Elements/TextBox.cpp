#include "TextBox.h"
#include "../../game.h"
#include "../UserInterface.h"

TextBox::TextBox(const char* text, Color textColor, Vector2 size, ArrowPlacement arrowPlacement, Vector2 position)
	: Element(position), text(text), textColor(textColor), size(size), arrowPlacement(arrowPlacement)
{
	Game::getGraphics().loadTexture("UI/textBoxCorner");
	Game::getGraphics().loadTexture("UI/textBoxSide");
	Game::getGraphics().loadTexture("UI/textBoxMiddle");
	Game::getGraphics().loadTexture("UI/textBoxArrow");
	this->textureCorner = Game::getGraphics().getTexturePointer("UI/textBoxCorner");
	this->textureSide = Game::getGraphics().getTexturePointer("UI/textBoxSide");
	this->textureMiddle = Game::getGraphics().getTexturePointer("UI/textBoxMiddle");
	this->textureArrow = Game::getGraphics().getTexturePointer("UI/textBoxArrow");
	assert(textureCorner && "Texture failed to load!");
	assert(textureSide && "Texture failed to load!");
	assert(textureMiddle && "Texture failed to load!");
	assert(textureArrow && "Texture failed to load!");
}

TextBox::~TextBox()
{
}

void TextBox::draw(bool selected)
{
	SpriteBatch* sb = UserInterface::getSpriteBatch();

	RECT middle = SimpleMath::Rectangle(this->position.x + this->textureSide->getHeight(), this->position.y + this->textureSide->getHeight(), this->size.x - 3 * this->textureSide->getHeight(), this->size.y - 3 * this->textureSide->getHeight());
	sb->Draw(this->textureMiddle->getShaderResView(), middle);

	sb->Draw(this->textureCorner->getShaderResView(), this->position);
	sb->Draw(this->textureCorner->getShaderResView(), this->position + Vector2(this->size.x - this->textureCorner->getWidth(), 0), nullptr, Colors::White, XM_PIDIV2);
	sb->Draw(this->textureCorner->getShaderResView(), this->position + this->size - this->textureCorner->getSize(), nullptr, Colors::White, XM_PI);
	sb->Draw(this->textureCorner->getShaderResView(), this->position + Vector2(0, this->size.y - this->textureCorner->getWidth()), nullptr, Colors::White, XM_PIDIV2 * 3);

	RECT top = SimpleMath::Rectangle(this->position.x + this->textureCorner->getWidth(), this->position.y, this->size.x - 3 * this->textureCorner->getWidth(), this->textureSide->getHeight());
	RECT bottom = SimpleMath::Rectangle(this->position.x + this->textureCorner->getWidth() + this->size.x - 3 * this->textureCorner->getWidth(), this->position.y + this->size.y - this->textureSide->getHeight(), this->size.x - 3 * this->textureCorner->getWidth(), this->textureSide->getHeight());
	RECT left = SimpleMath::Rectangle(this->position.x, this->position.y + this->textureCorner->getWidth() + this->size.y - 3 * this->textureCorner->getWidth(), this->size.y - 3 * this->textureCorner->getWidth(), this->textureSide->getHeight());
	RECT right = SimpleMath::Rectangle(this->position.x + this->size.x - this->textureSide->getHeight(), this->position.y + this->textureCorner->getWidth(), this->size.y - 3 * this->textureCorner->getWidth(), this->textureSide->getHeight());
	sb->Draw(this->textureSide->getShaderResView(), top);
	sb->Draw(this->textureSide->getShaderResView(), bottom, nullptr, Colors::White, XM_PI);
	sb->Draw(this->textureSide->getShaderResView(), left, nullptr, Colors::White, XM_PIDIV2 * 3);
	sb->Draw(this->textureSide->getShaderResView(), right, nullptr, Colors::White, XM_PIDIV2);

	if (this->arrowPlacement != ArrowPlacement::NONE)
	{
		Vector2 arrowPos;
		float arrowRot = 0.0f;
		switch (this->arrowPlacement)
		{
		case ArrowPlacement::TOP:
			arrowPos = Vector2(this->size.x / 2, 0);
			break;
		case ArrowPlacement::RIGHT:
			arrowPos = Vector2(this->size.x, this->size.y / 2);
			arrowRot = XM_PIDIV2;
			break;
		case ArrowPlacement::BOTTOM:
			arrowPos = Vector2(this->size.x / 2, this->size.y);
			arrowRot = XM_PI;
			break;
		case ArrowPlacement::LEFT:
			arrowPos = Vector2(0, this->size.y / 2);
			arrowRot = XM_PIDIV2 * 3;
			break;
		}

		sb->Draw(this->textureArrow->getShaderResView(), this->position + arrowPos, nullptr, Colors::White, arrowRot, this->textureArrow->getCenter());
	}

	UserInterface::getFontArial()->DrawString(sb, this->text, this->position + this->textureCorner->getSize(), this->textColor, 0, Vector2::Zero, 0.2f);
}
