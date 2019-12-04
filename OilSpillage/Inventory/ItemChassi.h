#ifndef ITEM_CHASSI_H
#define ITEM_CHASSI_H

#include "Item.h"
#include "../VehicleStats.h"
#include "../Weapon.h"

class ItemChassi : public Item
{
private:
	static std::string generateDescription(Stats chassi);
	Stats stats;
public:
	ItemChassi(std::string name, GameObject* object);
	virtual ~ItemChassi();
	ItemChassi(const ItemChassi& obj);
	Item* clone()const;
	bool operator==(const ItemChassi& other) const;

	virtual void randomize();

	Stats& getChassi();
	Stats getStats();
};

#endif // !ITEM_WEAPON_H
