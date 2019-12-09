#include "Item.h"
#include "ItemWeapon.h"
#include "ItemChassi.h"
#include "ItemGadget.h"
#include "ItemWheel.h"
#include "../game.h"
#include "../PG/defs.hpp"
std::vector<std::shared_ptr<Item>> Item::premadeItems;

float Item::fixedDecimals(float number, int decimals)
{
	return static_cast<int>(number * std::powf(10, decimals)) / std::powf(10, decimals);
}

GameObject* Item::getObjectByName(std::string name)
{
	for (int i = 0; i < Item::premadeItems.size(); i++)
	{
		if (Item::premadeItems[i]->name == name)
		{
			return Item::premadeItems[i]->object;
		}
	}

	return nullptr;
}

void Item::init()
{
	Graphics& graphics = Game::getGraphics();

	graphics.loadModel("Entities/Minigun");
	graphics.loadModel("Entities/FlameThrower");
	graphics.loadModel("Entities/Lazer");
	graphics.loadModel("Entities/Spike");
	graphics.loadModel("Entities/Nitro");
	graphics.loadModel("Entities/EMP");
	graphics.loadModel("Entities/Player_Car_Parts/Chassi1");
	graphics.loadModel("Entities/Player_Car_Parts/Wheel1");

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
	spike->setPosition(spike->mesh->getAABB().scale(spike->getScale()).maxPos * Vector3(0, 1, 0));

	GameObject* nitro = new GameObject();
	nitro->mesh = graphics.getMeshPointer("Entities/Nitro");
	nitro->setMaterial(graphics.getMaterial("Entities/Nitro"));
	nitro->setScale(Vector3(0.001f));
	nitro->setPosition(nitro->mesh->getAABB().scale(nitro->getScale()).maxPos * Vector3(0, 1, 0) + Vector3(0, 0.1f, 0));

	GameObject* emp = new GameObject();
	emp->mesh = graphics.getMeshPointer("Entities/EMP");
	emp->setMaterial(graphics.getMaterial("Entities/EMP"));
	emp->setScale(Vector3(0.0005f));
	emp->setPosition(emp->mesh->getAABB().scale(emp->getScale()).maxPos * Vector3(0, 1, 0) + Vector3(0, 0.1f, 0));

	GameObject* chassi1 = new GameObject();
	chassi1->mesh = graphics.getMeshPointer("Entities/Player_Car_Parts/Chassi1");
	chassi1->setMaterial(graphics.getMaterial("Entities/Player_Car_Parts/Chassi1"));
	chassi1->setScale(Vector3(0.05f));
	chassi1->setPosition(chassi1->mesh->getAABB().scale(chassi1->getScale()).maxPos * Vector3(0, 1, 0) + Vector3(0,0.1f,0));

	GameObject* wheel1 = new GameObject();
	wheel1->mesh = graphics.getMeshPointer("Entities/Player_Car_Parts/Wheel1");
	wheel1->setMaterial(graphics.getMaterial("Entities/Player_Car_Parts/Wheel1"));
	wheel1->setScale(Vector3(0.21f));
	wheel1->setPosition(wheel1->mesh->getAABB().scale(wheel1->getScale()).maxPos * Vector3(0, 1, 0) + Vector3(0, 0.1f, 0));

	Item::premadeItems = {
		std::make_shared<ItemWeapon>("Machinegun", WeaponHandler::getWeapon(WeaponType::MachineGun), machineGun),
		std::make_shared<ItemWeapon>("Flamethrower", WeaponHandler::getWeapon(WeaponType::Flamethrower), flameThrower),
		std::make_shared<ItemWeapon>("Lazer", WeaponHandler::getWeapon(WeaponType::Laser), lazer),
		std::make_shared<ItemWeapon>("Spikes", WeaponHandler::getWeapon(WeaponType::Spikes), spike),
		//std::make_shared<ItemGadget>("Nitro", GadgetHandler::getGadget(GadgetType::NITRO) ,nitro),
		//std::make_shared<ItemGadget>("EMP",GadgetHandler::getGadget(GadgetType::EMP), emp)//,
		std::make_shared<ItemChassi>("Muscle Chassi", 100, 1.0f, chassi1),
		std::make_shared<ItemWheel>("Muscle Tires", 1.0f, 1.0f, wheel1)
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
	if (!this->object)
	{
		this->baseColor = Vector4(0.0, 0.0, 0.0, 0.0);
	}
	else
	{
		this->baseColor = this->object->getColor();
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
