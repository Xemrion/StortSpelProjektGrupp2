#include "ItemSlot.h"
#include "../../game.h"
#include <cassert>
#include "../UserInterface.h"

Vector2 ItemSlot::size = Vector2(100, 100);

void ItemSlot::addTextbox()
{
	if (this->showTextBox)
	{
		this->textBox = std::make_unique<TextBox>("-- " + this->slot->item->getName() + " --\n" + this->slot->item->getDescription(), Color(Colors::Black), Vector2(), ArrowPlacement::TOP);
		this->textBox->setPosition(this->position + Vector2(ItemSlot::size.x * 0.5f - this->textBox->getSize().x * 0.5f, ItemSlot::size.y + 10.0f));
	}
}

ItemSlot::ItemSlot(Vector2 position, bool showTextBox) : Element(position), showTextBox(showTextBox), slot(nullptr), rotationTimer(0)
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

		if (this->textBox)
		{
			this->textBox->draw(selected);
		}
	}
}

void ItemSlot::update(bool selected, float deltaTime)
{
	if (this->slot && this->slot->item->getObject())
	{
		if (selected)
		{
			rotationTimer = std::fmodf(rotationTimer + deltaTime * 4, XM_2PI);
			//rotation = Quaternion::CreateFromYawPitchRoll(rotationTimer, 0.0f, 0.0f);
			rotation = Quaternion::Slerp(rotation, Quaternion::CreateFromYawPitchRoll(rotationTimer, 0, 0), deltaTime * 10);
			transform = Item::generateTransform(this->slot->item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f), Vector3(1.5f), rotation, true);
		}
		else
		{
			rotationTimer = Game::lerp(rotationTimer, 190 * XM_PI / 180, deltaTime * 4);
			rotation = Quaternion::Slerp(rotation, Quaternion::CreateFromYawPitchRoll(XM_PI + 0.3f, 0.26f, 0.0f), deltaTime * 4);
			transform = Item::generateTransform(this->slot->item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f), Vector3(1.5f), rotation,true);
		}
	}
}

Container::Slot* ItemSlot::getSlot() const
{
	return this->slot;
}

void ItemSlot::setSlot(Container::Slot* slot)
{
	if (this->slot)
	{
		if (this->slot->item->getObject())
		{
			Game::getGraphics().removeFromUIDraw(this->slot->item->getObject(), &this->transform);
		}

		this->textBox.reset();
	}

	this->slot = slot;

	if (slot)
	{
		if (this->slot->item->getObject())
		{
			rotationTimer = 190 * XM_PI / 180;
			rotation = Quaternion::CreateFromYawPitchRoll(XM_PI + 0.3f, 0.26f, 0.0f);
			transform = Item::generateTransform(this->slot->item->getObject(), this->position + Vector2(ItemSlot::size.x * 0.5f, ItemSlot::size.y - 10.0f), Vector3(1.0f), rotation, true);
			Game::getGraphics().addToUIDraw(this->slot->item->getObject(), &this->transform);
		}

		this->addTextbox();
	}
}
