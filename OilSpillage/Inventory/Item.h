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

	const char* name;
	const char* description;
	ItemType type;
	std::shared_ptr<GameObject> object;
public:
	static void init();
	static Item* getItemByName(const char* name);
	static Item* getRandom();
	static Matrix generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale = Vector3::One, Vector3 rotation = Vector3::Zero, bool ignoreObjectRotation = false);

	Item(const char* name, const char* description, ItemType type, std::shared_ptr<GameObject> object);
	virtual ~Item();
	Item(const Item& obj);
	/*void operator=(const Item& obj);*/

	const char* getName() const;
	const char* getDescription() const;
	ItemType getType() const;
	GameObject* getObject() const;
	void destroyObject();
};

#endif // !ITEM_H
