#ifndef UI_RANDOM_ITEM_H
#define UI_RANDOM_ITEM_H

#include "../UserInterface.h"
#include "../Elements/ItemSlot.h"

class UIRandomItem : public UserInterface
{
private:
	static const int slotCount = 3;

	void updateUI(float deltaTime);
	void drawUI();

	Texture* textureIndicator;
	Element* selected;
	std::unique_ptr<ItemSlot> slots[slotCount];
	Container::Slot container[slotCount];
public:
	UIRandomItem();
	virtual ~UIRandomItem();

	void init();
};

#endif // !UI_RANDOM_ITEM_H
