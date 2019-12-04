#ifndef ITEM_CHASSI_H
#define ITEM_CHASSI_H

#include "Item.h"

class ItemChassi : public Item
{
private:
	static std::string generateDescription(float maxHealth, float speed);

	float maxHealth;
	float speed;
public:
	ItemChassi(std::string name, float maxHealth, float speed, GameObject* object);
	virtual ~ItemChassi();
	ItemChassi(const ItemChassi& obj);
	Item* clone() const;

	virtual void randomize();

	float getMaxHealth() const;
	float getSpeed() const;
};

#endif // !ITEM_WEAPON_H
