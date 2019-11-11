#include "ItemSelector.h"
#include "../../game.h"
#include "../UserInterface.h"

Vector2 ItemSelector::size = Vector2(736, 192);

void ItemSelector::addTextbox()
{
	int selectedIndex = this->selectedIndex[this->selectedType] - this->startIndex[this->selectedType];
	Item* item = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->at(this->selectedIndex[this->selectedType]);
	this->textBox = std::make_unique<TextBox>("-- " + item->getName() + " --\n" + item->getDescription(), Color(Colors::Black), Vector2(), ArrowPlacement::BOTTOM);
	this->textBox->setPosition(this->position + Vector2(145.0f + 96.0f * selectedIndex - this->textBox->getSize().x * 0.5f, -this->textBox->getSize().y + 40.0f));
}

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

	if (this->textBox)
	{
		this->textBox->draw(false);
	}
}

void ItemSelector::update(float deltaTime)
{
	std::vector<Item*>* list = nullptr;

	if (this->selectedTypeLastDraw == -1)
	{
		this->addTextbox();
	}

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
				rotationTimers[i] = 0.0f;
				rotation[i] = Vector3();
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
			rotation[i] = Vector3(rotationTimers[i], 0.0f, 0.0f);
			transforms[i] = Item::generateTransform(object, this->position + Vector2(145.0f + 96.0f * i, 140.0f), Vector3(1.2f), rotation[i], false);
		}
		else
		{
			rotationTimers[i] = Game::lerp(rotationTimers[i], 0.0f, deltaTime * 4);
			rotation[i] = Vector3::Lerp(rotation[i], Vector3(), deltaTime * 4);
			transforms[i] = Item::generateTransform(object, this->position + Vector2(145.0f + 96.0f * i, 140.0f), Vector3(1.0f), rotation[i]);
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

	int listSize = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->size();
	if (listSize > 0)
	{
		this->addTextbox();
	}
	else
	{
		this->textBox.reset();
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

	this->addTextbox();
}

ItemType ItemSelector::getSelectedType() const
{
	return static_cast<ItemType>(this->selectedType);
}

Item* ItemSelector::getSelectedItem() const
{
	return Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->at(this->selectedIndex[this->selectedType]);
}
