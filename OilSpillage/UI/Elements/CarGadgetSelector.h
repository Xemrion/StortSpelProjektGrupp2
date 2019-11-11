#ifndef CAR_GADGET_SELECTOR_H
#define CAR_GATGET_SELECTOR_H

#include "Element.h"
#include "../../Vehicle.h"
#include "ItemSlot.h"

class CarGadgetSelector : public Element
{
private:
	Texture* textureBG;
	Element* selected;
	std::unique_ptr<ItemSlot> slots[Slots::SIZEOF];
public:
	static Vector2 size;

	CarGadgetSelector(Vector2 position = Vector2());
	virtual ~CarGadgetSelector();

	void draw(bool selected);
	void update(bool selected, float deltaTime);

	void setItemOfSelected(Item* item);
	Item* getItemOfSelected() const;
};

#endif // !CAR_GADGET_SELECTOR_H
