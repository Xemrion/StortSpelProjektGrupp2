#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>
#include "../VehicleStats.h"
#include "../GameObject.h"

enum ItemType
{
	TYPE_WEAPON,
	TYPE_GADGET,
	TYPE_CHASSI,
	TYPE_WHEEL,
	TYPES_SIZEOF
};

class Item
{
private:
	static std::vector<std::shared_ptr<Item>> premadeItems;
protected:
	std::string name;
	std::string description;
	ItemType type;
	Vector4 baseColor;
	GameObject* object;
public:
	static float fixedDecimals(float number, int decimals);
	static GameObject* getObjectByName(std::string name);
	static void init();
	static Item* getDefaultChassi();
	static Item* getDefaultWheels();
	static Item* getRandom();
	static Matrix generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale = Vector3::One, Quaternion rotation = Quaternion::Identity, bool ignoreObjectRotation = false);

	Item(std::string name, std::string description, ItemType type, GameObject* object);
	virtual ~Item();
	Item(const Item& obj);
	virtual Item* clone()const;
	virtual void randomize();

	std::string getName() const;
	std::string getDescription() const;
	ItemType getType() const;
	GameObject* getObject() const;
	Vector4 getBaseColor() const;

};

#endif // !ITEM_H
