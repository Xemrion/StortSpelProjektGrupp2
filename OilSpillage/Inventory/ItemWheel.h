#ifndef ITEM_WHEEL_H
#define ITEM_WHEEL_H

#include "Item.h"
#include "../VehicleStats.h"
#include "../Weapon.h"

class ItemWheel : public Item
{
private:
	static std::string generateDescription(float accelerationRate, float handlingRate);

	float accelerationRate;
	float handlingRate;
public:
	ItemWheel(std::string name, float accelerationRate, float handlingRate, GameObject* object);
	virtual ~ItemWheel();
	ItemWheel(const ItemWheel& obj);
	Item* clone()const;

	virtual void randomize();

	float getAccelerationRate() const;
	float getHandlingRate() const;
};

#endif // !ITEM_WEAPON_H
