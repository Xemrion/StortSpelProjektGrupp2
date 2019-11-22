#ifndef ITEM_SELECTOR_H
#define ITEM_SELECTOR_H

#include "Element.h"
#include "TextBox.h"
#include "../../Inventory/Container.h"
#include "../../Vehicle.h"

class ItemSelector : public Element
{
private:
	static const int tileLength = 6;
	void addTextbox();

	Container::Slot** used;

	Texture* textureBG;
	Texture* textureIndicator;
	std::unique_ptr<TextBox> textBox;

	float rotationTimers[tileLength];
	Quaternion rotation[tileLength];
	Matrix transforms[tileLength];

	int selectedTypeLastDraw;
	int selectedIndexLastDraw;
	int startIndexLastDraw;
	int selectedType;
	int selectedIndex[ItemType::TYPES_SIZE];
	int startIndex[ItemType::TYPES_SIZE];
public:
	static Vector2 size;

	ItemSelector(Vector2 position = Vector2());
	virtual ~ItemSelector();

	void draw(bool selected);
	void update(float deltaTime);

	void changeSelectedType(bool down);
	void changeSelectedIndex(bool right);
	ItemType getSelectedType() const;
	Container::Slot* getSelectedSlot() const;
	bool isSelectedValid() const;
	void setUsed(Container::Slot** used);
};


#endif // !ITEM_SELECTOR_H
