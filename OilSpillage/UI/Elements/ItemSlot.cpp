#include "ItemSlot.h"
#include "../../game.h"
#include <cassert>
#include "../UserInterface.h"

Vector2 ItemSlot::size = Vector2(100, 100);

ItemSlot::ItemSlot(Vector2 position) : Element(position), item(nullptr), rotationTimer(0)
{
	Game::getGraphics().loadTexture("UI/itemSlot");
	Game::getGraphics().loadTexture("UI/itemSelectorIndicator");
	this->textureSlot = Game::getGraphics().getTexturePointer("UI/itemSlot");
	this->textureIndicator = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicator");
	assert(textureSlot && "Texture failed to load!");
	assert(textureIndicator && "Texture failed to load!");
}

ItemSlot::~ItemSlot()
{
}

void ItemSlot::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureSlot->getShaderResView(), this->position);

	if (selected)
	{
		UserInterface::getSpriteBatch()->Draw(this->textureIndicator->getShaderResView(), this->position);
	}
}

void ItemSlot::update(bool selected, float deltaTime)
{
	if (item && item->getObject())
	{
		if (selected)
		{
			rotationTimer = std::fmodf(rotationTimer + deltaTime * 4, XM_2PI);
			rotation = Vector3(rotationTimer, 0.0f, 0.0f);
			transform = Item::generateTransform(item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f), Vector3(1.2f), rotation, false);
		}
		else
		{
			rotationTimer = Game::lerp(rotationTimer, 0.0f, deltaTime * 4);
			rotation = Vector3::Lerp(rotation, Vector3(), deltaTime * 4);
			transform = Item::generateTransform(item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f), Vector3(1.0f), rotation);
		}
	}
}

Item* ItemSlot::getItem() const
{
	return this->item;
}

void ItemSlot::setItem(Item* item)
{
	if (this->item && this->item->getObject())
	{
		Game::getGraphics().removeFromUIDraw(this->item->getObject(), &this->transform);
	}

	if (item && item->getObject())
	{
		rotationTimer = 0.0f;
		rotation = Vector3();
		transform = Item::generateTransform(item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f));
		Game::getGraphics().addToUIDraw(item->getObject(), &this->transform);
	}

	this->item = item;
}
