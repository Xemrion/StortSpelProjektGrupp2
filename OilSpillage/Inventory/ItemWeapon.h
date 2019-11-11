#ifndef ITEM_WEAPON_H
#define ITEM_WEAPON_H

#include "Item.h"
#include "../Weapon.h"

class ItemWeapon : public Item
{
private:
	static const char* generateDescription(const Weapon& weapon);

	Weapon weapon;
public:
	ItemWeapon(const char* name, Weapon weapon, std::shared_ptr<GameObject> object);
	~ItemWeapon();
	Weapon getWeapon() const;
};

#endif // !ITEM_WEAPON_H
