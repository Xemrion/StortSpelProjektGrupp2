#include "CheckBox.h"
#include "UserInterface.h"
#include "..//game.h"

Vector2 CheckBox::size = Vector2(64, 64);

CheckBox::CheckBox(Vector2 position) : Element(position, 0.0f), activated(false)
{
	Game::getGraphics().loadTexture("boxChecked.tga");
	Game::getGraphics().loadTexture("boxUnchecked.tga");
	this->textureChecked = Game::getGraphics().getTexturePointer("boxChecked.tga");
	this->textureUnchecked = Game::getGraphics().getTexturePointer("boxUnchecked.tga");
}

CheckBox::~CheckBox()
{
}

void CheckBox::draw(bool selected)
{
	ID3D11ShaderResourceView* texture = this->activated ? this->textureChecked->getShaderResView() : this->textureUnchecked->getShaderResView();
	UserInterface::getSpriteBatch()->Draw(texture, this->position, nullptr, selected ? Colors::Gray : Colors::White);
}

void CheckBox::setActivated(bool activated)
{
	this->activated = activated;
}

bool CheckBox::isActivated() const
{
	return this->activated;
}
