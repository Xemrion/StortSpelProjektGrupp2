#ifndef ITEM_WHEEL_H
#define ITEM_WHEEL_H

#include "Item.h"
#include "../VehicleStats.h"
#include "../Weapon.h"

class ItemWheel : public Item
{
private:
	static std::string generateDescription(Stats wheel);
	Stats stats;
public:
	ItemWheel(std::string name,GameObject* object);
	virtual ~ItemWheel();
	ItemWheel(const ItemWheel& obj);
	Item* clone()const;
	bool operator==(const ItemWheel& other) const;

	virtual void randomize();

	Stats& getWheel();
	Stats getStats();
};

#endif // !ITEM_WEAPON_H
