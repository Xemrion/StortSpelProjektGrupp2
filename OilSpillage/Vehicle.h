#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"
#include "Weapon.h"
#include "VehicleStats.h"
#include "Powerup.h"
#include "Sound.h"
#include "Lights.h"
#include "Inventory/Item.h"
#include "Inventory/ItemWeapon.h"
#include "Inventory/ItemWheel.h"
#include "Inventory/ItemChassi.h"
#include "Inventory/Container.h"
#include "Inventory/ItemGadget.h"

using namespace DirectX::SimpleMath;
enum Slots
{
	FRONT,
	LEFT,
	RIGHT,
	MOUNTED,
	BACK,
	WHEEL,
	CHASSI,
	SIZEOF
};
struct VehicleSlots
{
private:
	Item* slots[Slots::SIZEOF];
	Container::Slot* inventorySlots[Slots::SIZEOF];
public:
	void setSlot(Slots slot, Item* item, Container::Slot* inventorySlot)
	{
		if (this->slots[int(slot)] != nullptr)
		{
			delete this->slots[int(slot)];
		}

		this->slots[int(slot)] = item;
		this->inventorySlots[int(slot)] = inventorySlot;

		if (item != nullptr)
		{
			if (item->getObject() != nullptr)
			{
				item->getObject()->setRotation(Vector3(0,0.0f,0));
				item->getObject()->setScale(Vector3(0.25f));
				item->getObject()->setSpotShadow(false);
			}
		}
	};
	Item* getItem(Slots slot)
	{
		if (this->slots[int(slot)] != nullptr)
		{
			return this->slots[int(slot)];
		}
		else
		{
			return nullptr;
		}
	};
	Container::Slot* getInventorySlot(Slots slot)
	{
		if (this->slots[int(slot)] != nullptr)
		{
			return this->inventorySlots[int(slot)];
		}
		else
		{
			return nullptr;
		}
	};
	int checkForSlot(Container::Slot* inventorySlot)
	{
		for (int i = 0; i < Slots::SIZEOF; i++)
		{
			if (this->inventorySlots[i] == inventorySlot)
			{
				return i;
			}
		}

		return -1;
	};
	VehicleSlots()
	{
		for (int i = 0; i < Slots::SIZEOF; i++)
		{
			this->slots[i] = nullptr;
			this->inventorySlots[i] = nullptr;
		}
	};
	~VehicleSlots()
	{
		for (int i = 0; i < Slots::SIZEOF; i++)
		{
			if (this->slots[i] != nullptr)
			{
				delete this->slots[i];
			}
		}
	};
	VehicleSlots(const VehicleSlots& obj)
	{
		for (int i = 0; i < Slots::SIZEOF; i++)
		{
			if (obj.slots[i] != nullptr)
			{
				this->slots[i] = obj.slots[i]->clone();//will make new item/itemWeapon
				this->inventorySlots[i] = obj.inventorySlots[i];
			}
			else
			{
				this->slots[i] = nullptr;
				this->inventorySlots[i] = nullptr;
			}
		}
	}

};
class Vehicle : public GameObject
{
private:
	btScalar mRotation;
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;
	//btGeneric6DofSpring2Constraint* spring1;
	//btGeneric6DofSpring2Constraint* spring2;
	//btGeneric6DofSpring2Constraint* spring3;
	//btGeneric6DofSpring2Constraint* spring4;
	GameObject* vehicleBody1;
	btPoint2PointConstraint* pointJoint;
	//btRaycastVehicle* vehicleBody;
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster* m_vehicleRayCaster;
	btCollisionShape* m_wheelShape;
	SpotLight* spotLight;
	float health;
	bool deadImpulse;
	bool immortal;
	float immortalTimer;
	float respawnTimer;
	float totalRespawnTime;
	float powerUpTimers[(int)PowerUpType::Length];
	float particleTimer = 0.0f;

	VehicleSlots* vehicleSlots;

	float trailTimer = 0.0f;
	float driftForce = 0.0f;

	Stats defaultStats;
	Stats updatedStats;

	static const int bulletCount = 512;
	Bullet bullets[bulletCount];

	float gunRotation;
	DirectX::XMFLOAT2 velocity;
	float strength;
	float add;

	DirectX::XMFLOAT3 accelerator;
	float acceleratorTempX;
	float acceleratorTempZ;

	float targetRotation;
	float topSpeed;
	float counter = 0.0f;
	float rotateAcceleration = 0.0f;
	float rotationSmoother = 1.0f;
	float tempTargetRotation;
	bool rotationDirection;
	float cameraDistance;
	float cameraDistanceX;
	float cameraDistanceZ;
	Vector3 accelForce;
	float reverseTimer;
	float reverseTimer2;
	float soundTimer;
	bool flameBool;
	bool dmg;
	Vector2 tempCurDir;
	
	int soundHandle = 0;
	int driftHandle = 0;
	int starPowerHandle = 0;
	bool driftBool;
	float driftVolume;
	float enginePitch;
	float engineGears;
	int randomGears;
	float time;
	float wheelRotation;
	Vector2 aimLerp;
	Vector2 curDir;
	Vector2 currentDir;
	float velocitySimple;
	float velocitySpeed;
	class Physics* physics;
	Vector2 vehicleDirection;
	GameObject* empplaced;
	//Gadget
	bool nitroTrue;
	Vector2 targetRotation2;
public:
	float fireTimer;
	Vehicle();
	virtual ~Vehicle();

	void init(Physics* physics);
	void updatePlayer(float deltaTime);
	void update(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput);
	void updateWeapon(float deltaTime);
	void setVehicleSlots(VehicleSlots* slots);
	void setSpecSlot(Slots slot, Item* item, Container::Slot* inventorySlot);
	VehicleSlots* getSlots();
	GameObject* getVehicleBody1() { return this->vehicleBody1; }
	float getAcceleratorX();
	void startEngineSound();
	void stopEngineSound();

	Stats getStats()const; 

	void setSpotLight(SpotLight* theLight);
	SpotLight* getSpotLight();
	void setLaserLight(LaserLight* light);
	Vector3 getVelocity();
	float getVelocitySpeed();
	float getRotator();
	Vector3 getCameraDistance(float deltaTime);
	void setAccelForce(Vector3 accelForce, float deltaTime);
	void setWheelRotation(float deltaTime);
	void setChassi(ItemChassi* chassi);
	void setWheels(ItemWheel* wheel);
	//void onCollision(Vector2 direction);

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(float health);
	void setMaxHealth(float maxHealth);
	void resetHealth();
	void changeHealth(float amount);
	bool isDead() const;
	float getTotalRespawnTime()const;
	float getRespawnTimer()const;

	float getPitch(DirectX::XMVECTOR Quaternion);
	float getYaw(DirectX::XMVECTOR Quaternion);
	float getRoll(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);

	Bullet* getBulletArray(size_t& count);
	void addPowerUp(PowerUpType p);
	void updatePowerUpEffects(float deltaTime);
	float getPowerUpTimer(PowerUpType p);

	void onFire(float dt);
	void setFire();


};

#endif // !VEHICLE_H