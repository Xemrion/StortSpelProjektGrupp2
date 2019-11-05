#ifndef ITEM_WEAPON_H
#define ITEM_WEAPON_H

#include "Item.h"
#include "../Weapon.h"

class ItemWeapon : public Item
{
private:
	const char* generateDescription();
	Weapon weapon;
public:
	ItemWeapon(const char* name, Weapon weapon, GameObject* object);
	virtual ~ItemWeapon();

	Weapon getWeapon() const;
};

#endif // !ITEM_WEAPON_H
