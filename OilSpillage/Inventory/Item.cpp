#include "Item.h"
#include "ItemWeapon.h"
#include "../game.h"

std::shared_ptr<GameObject> Item::machineGun;
std::shared_ptr<GameObject> Item::flameThrower;
std::shared_ptr<GameObject> Item::lazer;

std::vector<std::shared_ptr<Item>> Item::allItems;

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

Item* Item::clone() const
{
	return new Item(*this);
}

void Item::init()
{
	Graphics& graphics = Game::getGraphics();

	graphics.loadModel("Entities/MiniGun");
	graphics.loadModel("Entities/FlameThrower");
	graphics.loadModel("Entities/Lazer");

	Item::machineGun = std::make_unique<GameObject>();
	Item::machineGun->mesh = graphics.getMeshPointer("Entities/MiniGun");
	Item::machineGun->setMaterial(graphics.getMaterial("Entities/MiniGun"));
	Item::machineGun->setScale(Vector3(0.05f));
	Item::machineGun->setPosition(Item::machineGun->mesh->getAABB().scale(Item::machineGun->getScale()).maxPos * Vector3(0, 1, 0));
	Item::machineGun->setRotation(Vector3(0.0f, 0.0f, 0.0f));

	Item::flameThrower = std::make_unique<GameObject>();
	Item::flameThrower->mesh = graphics.getMeshPointer("Entities/FlameThrower");
	Item::flameThrower->setMaterial(graphics.getMaterial("Entities/FlameThrower"));
	Item::flameThrower->setScale(Vector3(0.05f));
	Item::flameThrower->setPosition(Item::machineGun->mesh->getAABB().scale(Item::machineGun->getScale()).maxPos * Vector3(0, 1, 0));
	Item::flameThrower->setRotation(Vector3(0.0f, 0.0f, 0.0f));


	Item::lazer = std::make_unique<GameObject>();
	Item::lazer->mesh = graphics.getMeshPointer("Entities/Lazer");
	Item::lazer->setMaterial(graphics.getMaterial("Entities/Lazer"));
	Item::lazer->setScale(Vector3(0.05f));
	Item::lazer->setPosition(Item::machineGun->mesh->getAABB().scale(Item::machineGun->getScale()).maxPos * Vector3(0, 1, 0));
	Item::lazer->setRotation(Vector3(0.0f, 0.0f, 0.0f));

	Item::allItems = {
		std::make_shared<Item>("Test Item", "A very useless thing!", ItemType::GADGET, nullptr),
		std::make_shared<Item>("Test Item 2", "A very useless thing 2!", ItemType::GADGET, nullptr),
		std::make_shared<Item>("Test Item 3", "A very useless thing 3!", ItemType::GADGET, nullptr),
		std::make_shared<ItemWeapon>("Machinegun", WeaponHandler::getWeapon(WeaponType::MachineGun), Item::machineGun),
		std::make_shared<ItemWeapon>("Flamethrower", WeaponHandler::getWeapon(WeaponType::Flamethrower), Item::flameThrower),
		std::make_shared<ItemWeapon>("Lazer", WeaponHandler::getWeapon(WeaponType::Laser), Item::lazer)
	};

	//Sort so we can use getItemByName later if needed.
	std::sort(allItems.begin(), allItems.end(), [](const std::shared_ptr<Item>& a, const std::shared_ptr<Item>& b) { return std::strcmp(a->getName().c_str(), b->getName().c_str()) < 0; });
}

Item* Item::getItemByName(std::string name)
{
	auto item = std::lower_bound(allItems.begin(), allItems.end(), name, [](const std::shared_ptr<Item>& item, std::string name) { return std::strcmp(item->getName().c_str(), name.c_str()) < 0; });

	if (item != allItems.end())
	{
		return (*item).get();
	}

	return nullptr;
}

Item* Item::getRandom()
{
	return allItems[rand() % allItems.size()].get();
}

Matrix Item::generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale, Quaternion rotation, bool ignoreObjectRotation)
{
	Matrix transform = Matrix::CreateScale(scale * object->getScale());
	transform *= Matrix::CreateFromQuaternion(Quaternion::Concatenate(rotation, ignoreObjectRotation ? Quaternion() : object->getRotationQuaternion()));
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
