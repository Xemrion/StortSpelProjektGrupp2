#ifndef ITEM_SELECTOR_H
#define ITEM_SELECTOR_H

#include "Element.h"
#include "../../Inventory/Inventory.h"

class ItemSelector : public Element
{
private:
	static const int tileLength = 6;

	Texture* textureBG;
	Texture* textureIndicator;

	int selectedType;
	int selectedIndex[ItemType::TYPES_SIZE];
	int startIndex[ItemType::TYPES_SIZE];
public:
	static Vector2 size;

	ItemSelector(Vector2 position = Vector2());
	virtual ~ItemSelector();

	void draw(bool selected);

	void changeSelectedType(bool down);
	void changeSelectedIndex(bool right);
	ItemType getSelectedType() const;
	Item* getSelectedItem() const;
};


#endif // !ITEM_SELECTOR_H
