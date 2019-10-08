#include "CheckBox.h"
#include "UserInterface.h"
#include "..//game.h"
#include <cassert>

Vector2 CheckBox::size = Vector2(64, 64);

CheckBox::CheckBox(Vector2 position) : Element(position, 0.0f), activated(false)
{
	Game::getGraphics().loadTexture("UI/boxChecked");
	Game::getGraphics().loadTexture("UI/boxUnchecked");
	this->textureChecked   = Game::getGraphics().getTexturePointer("UI/boxChecked");
	this->textureUnchecked = Game::getGraphics().getTexturePointer("UI/boxUnchecked");
   assert( textureChecked   && "Texture failed to load!" );
   assert( textureUnchecked && "Texture failed to load!" );
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
