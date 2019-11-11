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
	Texture* textureSlot;
	Texture* textureIndicator;
	Item* item;

	float rotationTimer;
	Vector3 rotation;
	Matrix transform;
public:
	static Vector2 size;

	ItemSlot(Vector2 position = Vector2());
	virtual ~ItemSlot();

	void draw(bool selected);
	void update(bool selected, float deltaTime);

	Item* getItem() const;
	void setItem(Item* item);
};

#endif // !ITEM_SLOT_H
