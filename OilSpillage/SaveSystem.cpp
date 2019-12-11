#include "SaveSystem.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include "game.h"
#include "States/PlayingGameState.h"
#include <filesystem>

SaveSystem::SaveSystem()
{
}

SaveSystem::~SaveSystem()
{
}

void SaveSystem::set(const std::string& key, const std::string& value)
{
	if (key != "" && value != "")
	{
		this->saveString[key] = value;
	}
}

void SaveSystem::set(const std::string& key, int value)
{
	if (key != "")
	{
		this->saveIntergers[key] = value;
	}
}

void SaveSystem::set(const std::string& key, float value)
{
	if (key != "")
	{
		this->saveFloat[key] = value;
	}
}

std::string SaveSystem::getString(const std::string& key)
{
	if (this->saveString.count(key) && key != "")
	{
		return this->saveString.at(key);
	}
	else
	{
		return "";
	}
}

int SaveSystem::getInteger(const std::string& key)
{
	if (this->saveIntergers.count(key))
	{
		return this->saveIntergers.at(key);
	}
	else
	{
		return 0;
	}
}

float SaveSystem::getFloat(const std::string& key)
{
	if (this->saveFloat.count(key))
	{
		return this->saveFloat.at(key);
	}
	else
	{
		return 0.0f;
	}
}

bool SaveSystem::existsString(const std::string& key)
{
	return this->saveString.count(key);
}

bool SaveSystem::existsInteger(const std::string& key)
{
	return this->saveIntergers.count(key);
}

bool SaveSystem::existsFloat(const std::string& key)
{
	return this->saveFloat.count(key);
}

void SaveSystem::read(const std::string& filepath)
{
	this->clear();
	std::ifstream myfile(filepath);
	if (myfile.is_open() && !myfile.eof())
	{
		std::string	key, sValue;
		int intergerSize, stringSize, floatSize, iValue;
		float fValue;
		// read the nrOfIntergers & string in the file
		myfile >> key >> intergerSize;
		myfile >> key >> floatSize;
		myfile >> key >> stringSize;
		// loop over the stringsize
		for (size_t i = 0; i < stringSize; i++)
		{
			myfile >> key >> sValue;
			this->set(key, sValue);
		}
		for (size_t i = 0; i < floatSize; i++)
		{
			myfile >> key >> fValue;
			this->set(key, fValue);
		}
		for (size_t i = 0; i < intergerSize; i++)
		{
			myfile >> key >> iValue;
			this->set(key, iValue);
		}
	}
	myfile.close();
}

void SaveSystem::save(const std::string& filepath)
{
	std::ofstream myfile(filepath);
	myfile << "#nrOfIntergers " << this->saveIntergers.size() << std::endl;
	myfile <<"#nrOfFloats "<< this->saveFloat.size() << std::endl;
	myfile << "#nrOfStrings " << this->saveString.size() << std::endl;

	/*write to disk til we have nothing else to write*/
	if (this->saveString.size() > 0)
	{
		for (auto const& element : this->saveString)
		{
			myfile << element.first << " " << element.second << std::endl; //first - key, second -value
		}
	}
	if (this->saveFloat.size() > 0)
	{
		for (auto const& element : this->saveFloat)
		{
			myfile << element.first << " " << element.second << std::endl; //first - key, second -value
		}
	}
	if (this->saveIntergers.size() > 0)
	{
		for (auto const& element : this->saveIntergers)
		{
			myfile << element.first << " " << element.second << std::endl; //first - key, second -value
		}
	}
	myfile.close();
}

void SaveSystem::clear()
{
	this->saveString.clear();
	this->saveFloat.clear();
	this->saveIntergers.clear();
}

void SaveSystem::loadGame(int id, VehicleSlots* slots)
{
	GameInfo& info = Game::getGameInfo();
	Container::playerInventory = std::make_unique<Container>();
	Container::Slot* slot = nullptr;

	if (!std::filesystem::exists("file" + std::to_string(id) + ".sav"))
	{
		Container::playerInventory->addItem(Item::getDefaultChassi());
		slot = Container::playerInventory->getLatestAdded();
		slots->setSlot(Slots::CHASSI, slot->getItem()->clone(), slot);

		Container::playerInventory->addItem(Item::getDefaultWheels());
		slot = Container::playerInventory->getLatestAdded();
		slots->setSlot(Slots::WHEEL, slot->getItem()->clone(), slot);

		info.saveFileID = id;
		return;
	}

	SaveSystem saveSystem;

	saveSystem.read("file" + std::to_string(id) + ".sav");
	info.saveFileID = id;
	info.nrOfClearedStages = saveSystem.getInteger("GameStages");
	info.nrOfObjectives = saveSystem.getInteger("GameObjectives");
	info.highScoreTotal = saveSystem.getInteger("GameScore");
	info.time = saveSystem.getFloat("GameTime");
	Game::setLocalScale(saveSystem.getFloat("GameLocalScale"));

	if (saveSystem.getFloat("GameLocalScale") < 1.0f) Game::setLocalScale(1.0f + info.nrOfClearedStages * 0.05f); //Fix for old save files

	int itemSlots[] = { -1, -1, -1, -1, -1, -1, -1 };
	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		if (saveSystem.existsInteger("Slot" + std::to_string(i)))
		{
			itemSlots[i] = saveSystem.getInteger("Slot" + std::to_string(i));
		}
	}

	int itemCount = saveSystem.getInteger("GameItemCount");
	for (int i = 0; i < itemCount; i++)
	{
		std::string name = saveSystem.getString("Item" + std::to_string(i) + "Name");
		std::replace(name.begin(), name.end(), '$', '\n');
		std::replace(name.begin(), name.end(), '#', ' ');
		std::string className = saveSystem.getString("Item" + std::to_string(i) + "Class");

		GameObject* object = new GameObject(*Item::getObjectByName(name));
		if (object)
		{
			Vector4 color;
			color.x = saveSystem.getFloat("Item" + std::to_string(i) + "BaseColorX");
			color.y = saveSystem.getFloat("Item" + std::to_string(i) + "BaseColorY");
			color.z = saveSystem.getFloat("Item" + std::to_string(i) + "BaseColorZ");
			object->setColor(color);
		}
		
		if (className == "ItemWeapon")
		{
			Weapon weapon;
			weapon.damage = saveSystem.getFloat("Item" + std::to_string(i) + "Damage");
			weapon.fireRate = saveSystem.getFloat("Item" + std::to_string(i) + "FireRate");
			weapon.bulletSpeed = saveSystem.getFloat("Item" + std::to_string(i) + "BulletSpeed");
			weapon.bulletLifetime = saveSystem.getFloat("Item" + std::to_string(i) + "BulletLifetime");
			weapon.bulletScale.x = saveSystem.getFloat("Item" + std::to_string(i) + "BulletScaleX");
			weapon.bulletScale.y = saveSystem.getFloat("Item" + std::to_string(i) + "BulletScaleY");
			weapon.bulletScale.z = saveSystem.getFloat("Item" + std::to_string(i) + "BulletScaleZ");
			weapon.spreadRadians = saveSystem.getFloat("Item" + std::to_string(i) + "SpreadRadians");
			weapon.maxSpread = saveSystem.getFloat("Item" + std::to_string(i) + "MaxSpread");
			weapon.spreadIncreasePerSecond = saveSystem.getFloat("Item" + std::to_string(i) + "SpreadIncrease");
			weapon.spreadDecreasePerSecond = saveSystem.getFloat("Item" + std::to_string(i) + "SpreadDecrease");
			weapon.type = static_cast<WeaponType>(saveSystem.getInteger("Item" + std::to_string(i) + "WeaponType"));
			weapon.lightColor.x = saveSystem.getFloat("Item" + std::to_string(i) + "LightColorR");
			weapon.lightColor.y = saveSystem.getFloat("Item" + std::to_string(i) + "LightColorG");
			weapon.lightColor.z = saveSystem.getFloat("Item" + std::to_string(i) + "LightColorB");
			weapon.melee = saveSystem.getInteger("Item" + std::to_string(i) + "Melee") == 1;
			weapon.doesDoT = saveSystem.getInteger("Item" + std::to_string(i) + "DoT") == 1;
			weapon.doTTimer = saveSystem.getFloat("Item" + std::to_string(i) + "DoTTimer");
			weapon.doesSplashDmg = saveSystem.getInteger("Item" + std::to_string(i) + "Splash") == 1;
			weapon.splashRange = saveSystem.getFloat("Item" + std::to_string(i) + "SplashRange");
			weapon.doesKnockBack = saveSystem.getInteger("Item" + std::to_string(i) + "Knockback") == 1;
			weapon.knockbackForce = saveSystem.getFloat("Item" + std::to_string(i) + "KnockbackForce");

			Container::playerInventory->addItem(new ItemWeapon(name, weapon, object));
		}
		else if (className == "ItemWheel")
		{
			float acceleration = saveSystem.getFloat("Item" + std::to_string(i) + "AccelRate");
			float handling = saveSystem.getFloat("Item" + std::to_string(i) + "Handling");

			Container::playerInventory->addItem(new ItemWheel(name, acceleration, handling, object));
		}
		else if (className == "ItemChassi")
		{
			float maxHealth = saveSystem.getFloat("Item" + std::to_string(i) + "MaxHealth");
			float topSpeed = saveSystem.getFloat("Item" + std::to_string(i) + "TopSpeed");

			Container::playerInventory->addItem(new ItemChassi(name, maxHealth, topSpeed, object));
		}
		else if (className == "ItemGadget")
		{
			Gadget gadget;
			gadget.lifeTime = saveSystem.getFloat("Item" + std::to_string(i) + "Lifetime");
			gadget.type = static_cast<GadgetType>(saveSystem.getInteger("Item" + std::to_string(i) + "Type"));
			gadget.radius = saveSystem.getFloat("Item" + std::to_string(i) + "Radius");
			gadget.cooldown = saveSystem.getFloat("Item" + std::to_string(i) + "Cooldown");
			gadget.power = saveSystem.getFloat("Item" + std::to_string(i) + "Power");

			Container::playerInventory->addItem(new ItemGadget(name, gadget, object));
		}
		else
		{
			std::string description = saveSystem.getString("Item" + std::to_string(i) + "Desc");
			ItemType type = static_cast<ItemType>(saveSystem.getInteger("Item" + std::to_string(i) + "Type"));

			std::replace(description.begin(), description.end(), '$', '\n');
			std::replace(description.begin(), description.end(), '#', ' ');

			Container::playerInventory->addItem(new Item(name, description, type, object));
		}

		for (int j = 0; j < Slots::SIZEOF; j++)
		{
			if (itemSlots[j] == i)
			{
				slot = Container::playerInventory->getLatestAdded();
				slots->setSlot(static_cast<Slots>(j), slot->getItem()->clone(), slot);
			}
		}
	}
}

void SaveSystem::saveGame(VehicleSlots* slots)
{
	SaveSystem saveSystem;
	int itemId = 0;
	GameInfo info = Game::getGameInfo();

	saveSystem.set("GameItemCount", Container::playerInventory->getItemCount());
	saveSystem.set("GameStages", info.nrOfClearedStages);
	saveSystem.set("GameObjectives", info.nrOfObjectives);
	saveSystem.set("GameScore", info.highScoreTotal);
	saveSystem.set("GameTime", static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	saveSystem.set("GameLocalScale", Game::getLocalScale());

	for (int i = 0; i < ItemType::TYPES_SIZEOF; i++)
	{
		auto stack = Container::playerInventory->getItemStack(static_cast<ItemType>(i));

		for (int j = 0; j < stack->size(); j++)
		{
			Item* item = (*stack)[j]->getItem();

			std::string name = item->getName();
			std::replace(name.begin(), name.end(), '\n', '$');
			std::replace(name.begin(), name.end(), ' ', '#');
			saveSystem.set("Item" + std::to_string(itemId) + "Name", name);
			saveSystem.set("Item" + std::to_string(itemId) + "BaseColorX", item->getBaseColor().x);
			saveSystem.set("Item" + std::to_string(itemId) + "BaseColorY", item->getBaseColor().y);
			saveSystem.set("Item" + std::to_string(itemId) + "BaseColorZ", item->getBaseColor().z);

			ItemWeapon* itemWeapon = dynamic_cast<ItemWeapon*>(item);
			ItemWheel* itemWheel = dynamic_cast<ItemWheel*>(item);
			ItemChassi* itemChassi = dynamic_cast<ItemChassi*>(item);
			ItemGadget* itemGadget = dynamic_cast<ItemGadget*>(item);
			if (itemWeapon)
			{
				Weapon weapon = itemWeapon->getWeapon();
				saveSystem.set("Item" + std::to_string(itemId) + "Damage", weapon.damage);
				saveSystem.set("Item" + std::to_string(itemId) + "FireRate", weapon.fireRate);
				saveSystem.set("Item" + std::to_string(itemId) + "BulletSpeed", weapon.bulletSpeed);
				saveSystem.set("Item" + std::to_string(itemId) + "BulletLifetime", weapon.bulletLifetime);
				saveSystem.set("Item" + std::to_string(itemId) + "BulletScaleX", weapon.bulletScale.x);
				saveSystem.set("Item" + std::to_string(itemId) + "BulletScaleY", weapon.bulletScale.y);
				saveSystem.set("Item" + std::to_string(itemId) + "BulletScaleZ", weapon.bulletScale.z);
				saveSystem.set("Item" + std::to_string(itemId) + "SpreadRadians", weapon.spreadRadians);
				saveSystem.set("Item" + std::to_string(itemId) + "MaxSpread", weapon.maxSpread);
				saveSystem.set("Item" + std::to_string(itemId) + "SpreadIncrease", weapon.spreadIncreasePerSecond);
				saveSystem.set("Item" + std::to_string(itemId) + "SpreadDecrease", weapon.spreadDecreasePerSecond);
				saveSystem.set("Item" + std::to_string(itemId) + "WeaponType", static_cast<int>(weapon.type));
				saveSystem.set("Item" + std::to_string(itemId) + "LightColorR", weapon.lightColor.x);
				saveSystem.set("Item" + std::to_string(itemId) + "LightColorG", weapon.lightColor.y);
				saveSystem.set("Item" + std::to_string(itemId) + "LightColorB", weapon.lightColor.z);
				saveSystem.set("Item" + std::to_string(itemId) + "Melee", weapon.melee ? 1 : 0);
				saveSystem.set("Item" + std::to_string(itemId) + "DoT", weapon.doesDoT ? 1 : 0);
				saveSystem.set("Item" + std::to_string(itemId) + "DoTTimer", weapon.doTTimer);
				saveSystem.set("Item" + std::to_string(itemId) + "Splash", weapon.doesSplashDmg ? 1 : 0);
				saveSystem.set("Item" + std::to_string(itemId) + "SplashRange", weapon.splashRange);
				saveSystem.set("Item" + std::to_string(i) + "Knockback", weapon.doesKnockBack ? 1 : 0);
				saveSystem.set("Item" + std::to_string(i) + "KnockbackForce", weapon.knockbackForce);

				saveSystem.set("Item" + std::to_string(itemId) + "Class", "ItemWeapon");
			}
			else if (itemWheel)
			{
				saveSystem.set("Item" + std::to_string(itemId) + "AccelRate", itemWheel->getAccelerationRate());
				saveSystem.set("Item" + std::to_string(itemId) + "Handling", itemWheel->getHandlingRate());

				saveSystem.set("Item" + std::to_string(itemId) + "Class", "ItemWheel");
			}
			else if (itemChassi)
			{
				saveSystem.set("Item" + std::to_string(itemId) + "MaxHealth", itemChassi->getMaxHealth());
				saveSystem.set("Item" + std::to_string(itemId) + "TopSpeed", itemChassi->getSpeed());

				saveSystem.set("Item" + std::to_string(itemId) + "Class", "ItemChassi");
			}
			else if (itemGadget)
			{
				Gadget gadget = itemGadget->getGadget();
				saveSystem.set("Item" + std::to_string(itemId) + "Lifetime", gadget.lifeTime);
				saveSystem.set("Item" + std::to_string(itemId) + "Type", gadget.type);
				saveSystem.set("Item" + std::to_string(itemId) + "Radius", gadget.radius);
				saveSystem.set("Item" + std::to_string(itemId) + "Cooldown", gadget.cooldown);
				saveSystem.set("Item" + std::to_string(itemId) + "Power", gadget.power);

				saveSystem.set("Item" + std::to_string(itemId) + "Class", "ItemGadget");
			}
			else
			{
				std::string description = item->getDescription();
				std::replace(description.begin(), description.end(), '\n', '$');
				std::replace(description.begin(), description.end(), ' ', '#');
				saveSystem.set("Item" + std::to_string(itemId) + "Type", item->getType());
				saveSystem.set("Item" + std::to_string(itemId) + "Desc", description);

				saveSystem.set("Item" + std::to_string(itemId) + "Class", "Item");
			}

			int slot = slots->checkForSlot((*stack)[j]);
			if (slot != -1)
			{
				saveSystem.set("Slot" + std::to_string(slot), itemId);

				if (slot == 1 || slot == 2)
				{
					saveSystem.set("Slot" + std::to_string(slot == 1 ? 2 : 1), itemId);
				}
			}

			itemId++;
		}
	}

	saveSystem.save("file" + std::to_string(info.saveFileID) + ".sav");
}

std::string SaveSystem::fileInfo(int id)
{
	if (!std::filesystem::exists("file" + std::to_string(id) + ".sav"))
	{
		return "New Game " + std::to_string(id + 1);
	}

	SaveSystem saveSystem;
	saveSystem.read("file" + std::to_string(id) + ".sav");
	int stages = saveSystem.getInteger("GameStages");
	int score = saveSystem.getInteger("GameScore");
	int time = static_cast<int>(saveSystem.getFloat("GameTime"));
	int minutes = time / 60;
	int seconds = time % 60;

	std::stringstream strStream;
	strStream << "Act " << (((stages - 1) / 3) + 1);
	strStream << " - Stage " << ((stages - 1) % 3 + 1) << '\n';
	strStream << "Score: " << score;
	strStream << " / Time: ";

	if (minutes < 10) strStream << "0";
	strStream << minutes << ":";
	if (seconds < 10) strStream << "0";
	strStream << seconds;

	return strStream.str();
}
