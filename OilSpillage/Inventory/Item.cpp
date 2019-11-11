#include "Item.h"
#include "ItemWeapon.h"
#include "../game.h"

std::shared_ptr<GameObject> Item::machineGun;
std::vector<Item> Item::allItems;

Item::Item(std::string name, std::string description, ItemType type, std::shared_ptr<GameObject> object)
	: name(name), description(description), type(type), object(object)
{
}

Item::~Item()
{

}

Item::Item(const Item& obj)
{
	this->name = obj.name;
	this->description = obj.description;
	this->type = obj.type;
	
	if (obj.object != nullptr)
	{
		this->object = std::make_shared<GameObject>(*obj.object.get());
	}
	else
	{
		this->object = nullptr;
	}
}

void Item::init()
{
	Graphics& graphics = Game::getGraphics();

	graphics.loadModel("Entities/Dummy_Turret");
	Item::machineGun = std::make_unique<GameObject>();
	Item::machineGun->mesh = graphics.getMeshPointer("Entities/Dummy_Turret1");
	Item::machineGun->setMaterial(graphics.getMaterial("Entities/Dummy_Turret"));
	Item::machineGun->setScale(Vector3(0.001f));
	Item::machineGun->setPosition(Item::machineGun->mesh->getAABB().scale(Item::machineGun->getScale()).maxPos * Vector3(0, 1, 0));
	Item::machineGun->setRotation(Vector3(1.5f, 0.3f, 0.8f));

	Item::allItems = {
		Item("Test Item", "A very useless thing!", ItemType::WEAPON, nullptr),
		Item("Test Item 2", "A very useless thing 2!", ItemType::GADGET, nullptr),
		Item("Test Item 3", "A very useless thing 3!", ItemType::GADGET, nullptr),
		ItemWeapon("Machine Gun", WeaponHandler::getWeapon(WeaponType::MachineGun), Item::machineGun)
	};

	//Sort so we can use getItemByName later if needed.
	std::sort(allItems.begin(), allItems.end(), [](const Item& a, const Item& b) { return std::strcmp(a.getName().c_str(), b.getName().c_str()) < 0; });
}

Item* Item::getItemByName(std::string name)
{
	auto item = std::lower_bound(allItems.begin(), allItems.end(), name, [](const Item& item, std::string name) { return std::strcmp(item.getName().c_str(), name.c_str()) < 0; });

	if (item != allItems.end())
	{
		return &(*item);
	}

	return nullptr;
}

Item* Item::getRandom()
{
	return &allItems[rand() % allItems.size()];
}

Matrix Item::generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale, Vector3 rotation, bool ignoreObjectRotation)
{
	Matrix transform = Matrix::CreateScale(scale * object->getScale());
	transform *= Matrix::CreateFromQuaternion(Quaternion::Concatenate(Quaternion::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z), ignoreObjectRotation ? Quaternion() : object->getRotationQuaternion()));
	transform *= Matrix::CreateTranslation(Game::getGraphics().screenToWorldSpaceUI(screenPos) + object->getPosition());
	
	return transform;
}

std::string Item::getName() const
{
	return this->name;
}

std::string Item::getDescription() const
{
	return this->description;
}

ItemType Item::getType() const
{
	return this->type;
}

GameObject* Item::getObject() const
{
	return this->object.get();
}
