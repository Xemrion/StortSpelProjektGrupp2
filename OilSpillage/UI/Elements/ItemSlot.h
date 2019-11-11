#ifndef ITEM_SLOT_H
#define ITEM_SLOT_H

#include "Element.h"
#include "../../Texture.h"
#include "../../Inventory/Item.h"
#include "TextBox.h"


class ItemSlot : public Element
{
private:
	void addTextbox();

	std::unique_ptr<TextBox> textBox;
	bool showTextBox;
	Texture* textureSlot;
	Texture* textureIndicator;
	Item* item;

	float rotationTimer;
	Quaternion rotation;
	Matrix transform;
public:
	static Vector2 size;

	ItemSlot(Vector2 position = Vector2(), bool showTextBox = true);
	virtual ~ItemSlot();

	void draw(bool selected);
	void update(bool selected, float deltaTime);

	Item* getItem() const;
	void setItem(Item* item);
};

#endif // !ITEM_SLOT_H
