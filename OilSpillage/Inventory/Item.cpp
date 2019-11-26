#include "Item.h"
#include "ItemWeapon.h"
#include "../game.h"
#include "../PG/defs.hpp"

std::vector<std::shared_ptr<Item>> Item::premadeItems;

void Item::init()
{
	Graphics& graphics = Game::getGraphics();

	graphics.loadModel("Entities/Minigun");
	graphics.loadModel("Entities/FlameThrower");
	graphics.loadModel("Entities/Lazer");
	graphics.loadModel("Entities/Spike");

	GameObject* machineGun = new GameObject();
	machineGun->mesh = graphics.getMeshPointer("Entities/Minigun");
	machineGun->setMaterial(graphics.getMaterial("Entities/Minigun"));
	machineGun->setScale(Vector3(0.04f));
	machineGun->setPosition(machineGun->mesh->getAABB().scale(machineGun->getScale()).maxPos * Vector3(0, 1, 0) + Vector3(0, 0.08f, 0));

	GameObject* flameThrower = new GameObject();
	flameThrower->mesh = graphics.getMeshPointer("Entities/FlameThrower");
	flameThrower->setMaterial(graphics.getMaterial("Entities/FlameThrower"));
	flameThrower->setScale(Vector3(0.05f));
	flameThrower->setPosition(flameThrower->mesh->getAABB().scale(flameThrower->getScale()).maxPos * Vector3(0, 1, 0));

	GameObject* lazer = new GameObject();
	lazer->mesh = graphics.getMeshPointer("Entities/Lazer");
	lazer->setMaterial(graphics.getMaterial("Entities/Lazer"));
	lazer->setScale(Vector3(0.05f));
	lazer->setPosition(lazer->mesh->getAABB().scale(lazer->getScale()).maxPos * Vector3(0, 1, 0));

	GameObject* spike = new GameObject();
	spike->mesh = graphics.getMeshPointer("Entities/Spike");
	spike->setMaterial(graphics.getMaterial("Entities/Spike"));
	spike->setScale(Vector3(0.05f));
	spike->setPosition(lazer->mesh->getAABB().scale(lazer->getScale()).maxPos * Vector3(0, 1, 0));

	Item::premadeItems = {
		std::make_shared<ItemWeapon>("Machinegun", WeaponHandler::getWeapon(WeaponType::MachineGun), machineGun),
		std::make_shared<ItemWeapon>("Flamethrower", WeaponHandler::getWeapon(WeaponType::Flamethrower), flameThrower),
		std::make_shared<ItemWeapon>("Lazer", WeaponHandler::getWeapon(WeaponType::Laser), lazer),
		std::make_shared<ItemWeapon>("Spikes", WeaponHandler::getWeapon(WeaponType::Spikes), spike)
	};

}

Item* Item::getRandom()
{
	Item* item = premadeItems[rand() % premadeItems.size()].get()->clone();
	item->randomize();

	return item;
}

Matrix Item::generateTransform(GameObject* object, Vector2 screenPos, Vector3 scale, Quaternion rotation, bool ignoreObjectRotation)
{
	Matrix transform = Matrix::CreateScale(scale * object->getScale());
	transform *= Matrix::CreateFromQuaternion(Quaternion::Concatenate(rotation, ignoreObjectRotation ? Quaternion() : object->getRotationQuaternion()));
	transform *= Matrix::CreateTranslation(Game::getGraphics().screenToWorldSpaceUI(screenPos) + object->getPosition());

	return transform;
}

Item::Item(std::string name, std::string description, ItemType type, GameObject* object)
	: name(name), description(description), type(type), object(object)
{
	this->baseColor = Vector4(0.0, 0.0, 0.0, 0.0);
	if (this->object != nullptr)
	{
		this->object->setColor(baseColor);
	}
}

Item::~Item()
{
	delete object;
}

Item::Item(const Item& obj)
{
	this->name = obj.name;
	this->description = obj.description;
	this->type = obj.type;
	this->baseColor = obj.baseColor;
	
	if (obj.object != nullptr)
	{
		this->object = new GameObject(*obj.object);
		this->object->setColor(baseColor);
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

bool Item::operator==(const Item& other) const
{
	return this->name == other.name && this->description == other.description && this->type == other.type && this->baseColor == other.baseColor;
}

void Item::randomize()
{
	F32_Dist genValue{ -.20f, .20f };
	RNG rng { RD()() };
	this->baseColor = Vector4(genValue(rng), genValue(rng), genValue(rng), 1);
	if (this->object)
	{
		this->object->setColor(baseColor);
	}
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
	return this->object;
}

Vector4 Item::getBaseColor() const
{
	return this->baseColor;
}
