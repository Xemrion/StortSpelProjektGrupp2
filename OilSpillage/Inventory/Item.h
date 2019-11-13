#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>
#include "../GameObject.h"

enum ItemType
{
	WEAPON,
	GADGET,
	CHASSI,
	WHEEL,
	TYPES_SIZE
};

class Item
{
private:
	static std::shared_ptr<GameObject> machineGun;
	static std::vector<Item> allItems;
protected:
	std::string name;
	std::string description;
	ItemType type;
	std::shared_ptr<GameObject> object;
public:
	static void init();
	static Item* getItemByName(std::string name);
	static Item* getRandom();
	static Matrix generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale = Vector3::One, Quaternion rotation = Quaternion::Identity, bool ignoreObjectRotation = false);

	Item(std::string name, std::string description, ItemType type, std::shared_ptr<GameObject> object);
	virtual ~Item();
	Item(const Item& obj);
	virtual Item* clone()const;
	/*void operator=(const Item& obj);*/

	std::string getName() const;
	std::string getDescription() const;
	ItemType getType() const;
	GameObject* getObject() const;
};

#endif // !ITEM_H
