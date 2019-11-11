#ifndef ITEM_WEAPON_H
#define ITEM_WEAPON_H

#include "Item.h"
#include "../Weapon.h"

class ItemWeapon : public Item
{
private:
	static std::string generateDescription(Weapon weapon);

	Weapon weapon;
public:
	ItemWeapon(std::string name, Weapon weapon, std::shared_ptr<GameObject> object);
	virtual ~ItemWeapon();

	Weapon getWeapon() const;
};

#endif // !ITEM_WEAPON_H