#include "ItemSelector.h"
#include "../../game.h"
#include "../UserInterface.h"

Vector2 ItemSelector::size = Vector2(512, 128);

ItemSelector::ItemSelector(Vector2 position) : Element(position), selectedTypeLastDraw(-1), selectedIndexLastDraw(-1), startIndexLastDraw(-1), selectedType(0), selectedIndex{ 0 }, startIndex{ 0 }, transforms{ Matrix() }, rotationTimers{ 0 }
{
	Game::getGraphics().loadTexture("UI/itemSelectorBG");
	Game::getGraphics().loadTexture("UI/itemSelectorIndicator");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/itemSelectorBG");
	this->textureIndicator = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicator");
	assert(textureBG && "Texture failed to load!");
	assert(textureIndicator && "Texture failed to load!");
}

ItemSelector::~ItemSelector()
{
}

void ItemSelector::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), this->position);
	UserInterface::getSpriteBatch()->Draw(this->textureIndicator->getShaderResView(), this->position + Vector2(96.0f + 96.0f * (this->selectedIndex[this->selectedType] - this->startIndex[this->selectedType]), 47.0f));

	/*for (int i = 0; i < min(list->size() - this->startIndex[this->selectedType], ItemSelector::tileLength); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), (*list)[this->startIndex[this->selectedType] + i]->getName(), this->position + Vector2(95.0f + 96.0f * i, 47.0f + 24.0f), Colors::Red, 0, Vector2(), 0.2f);
	}*/
}

void ItemSelector::update(float deltaTime)
{
	std::vector<Item*>* list = nullptr;

	if (this->selectedTypeLastDraw != this->selectedType || this->startIndexLastDraw != this->startIndex[this->selectedType])
	{
		if (this->selectedTypeLastDraw != -1)
		{
			list = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedTypeLastDraw));

			for (int i = 0; i < min(list->size() - this->startIndexLastDraw, ItemSelector::tileLength); i++)
			{
				GameObject* object = list->at(this->startIndexLastDraw + i)->getObject();
				if (object)
				{
					Game::getGraphics().removeFromUIDraw(object, &transforms[i]);
				}
			}
		}

		list = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType));

		for (int i = 0; i < min(list->size() - this->startIndex[this->selectedType], ItemSelector::tileLength); i++)
		{
			GameObject* object = list->at(this->startIndex[this->selectedType] + i)->getObject();
			if (object)
			{
				rotationTimers[i] = 190 * XM_PI/180;
				rotation[i] = Quaternion::CreateFromYawPitchRoll(XM_PI+ 0.3f, 0.26f, 0.0f);
				transforms[i] = Item::generateTransform(object, this->position + Vector2(145.0f + 96.0f * i, 140.0f));
				Game::getGraphics().addToUIDraw(object, &transforms[i]);
			}
		}

	}

	int selectedIndex = this->selectedIndex[this->selectedType] - this->startIndex[this->selectedType];
	list = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType));

	for (int i = 0; i < min(list->size(), ItemSelector::tileLength); i++)
	{
		GameObject* object = list->at(this->startIndex[this->selectedType] + i)->getObject();
		if (object == nullptr) continue;

		if (i == selectedIndex)
		{
			rotationTimers[i] = std::fmodf(rotationTimers[i] + deltaTime * 4, XM_2PI);
			rotation[i] = Quaternion::Slerp(rotation[i], Quaternion::CreateFromYawPitchRoll(rotationTimers[i],0,0), deltaTime * 10);
			transforms[i] = Item::generateTransform(object, this->position + Vector2(145.0f + 96.0f * i, 140.0f), Vector3(1.5f), rotation[i], true);
		}
		else
		{
			rotationTimers[i] = Game::lerp(rotationTimers[i], 190 * XM_PI / 180, deltaTime * 4);
			rotation[i] = Quaternion::Slerp(rotation[i], Quaternion::CreateFromYawPitchRoll(XM_PI + 0.3f, 0.26f, 0.0f), deltaTime * 4);
			transforms[i] = Item::generateTransform(object, this->position + Vector2(145.0f + 96.0f * i, 140.0f), Vector3(1.5f), rotation[i],true);
		}
	}

	this->selectedTypeLastDraw = this->selectedType;
	this->selectedIndexLastDraw = this->selectedIndex[this->selectedType];
	this->startIndexLastDraw = this->startIndex[this->selectedType];
}

void ItemSelector::changeSelectedType(bool down)
{
	if (down)
	{
		this->selectedType = (this->selectedType + 1) % ItemType::TYPES_SIZE;
	}
	else
	{
		this->selectedType = (this->selectedType - 1 + ItemType::TYPES_SIZE) % ItemType::TYPES_SIZE;
	}
}

void ItemSelector::changeSelectedIndex(bool right)
{
	int listSize = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->size();
	if (listSize == 0)
	{ 
		this->selectedIndex[this->selectedType] = 0;
		return;
	}
	
	if (right)
	{
		this->selectedIndex[this->selectedType] = (this->selectedIndex[this->selectedType] + 1) % listSize;
	}
	else
	{
		this->selectedIndex[this->selectedType] = (this->selectedIndex[this->selectedType] - 1 + listSize) % listSize;
	}

	if (this->selectedIndex[this->selectedType] < this->startIndex[this->selectedType])
	{
		this->startIndex[this->selectedType] = this->selectedIndex[this->selectedType];
	}
	else if (this->selectedIndex[this->selectedType] >= this->startIndex[this->selectedType] + ItemSelector::tileLength)
	{
		this->startIndex[this->selectedType] = this->selectedIndex[this->selectedType] - (ItemSelector::tileLength - 1);
	}
}

ItemType ItemSelector::getSelectedType() const
{
	return static_cast<ItemType>(this->selectedType);
}

Item* ItemSelector::getSelectedItem() const
{
	return Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->at(this->selectedIndex[this->selectedType]);
}
