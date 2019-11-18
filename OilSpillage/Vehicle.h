#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"
#include "Weapon.h"
#include "VehicleStats.h"
#include "Powerup.h"
#include "Sound.h"
#include"Lights.h"
using namespace DirectX::SimpleMath;

class Vehicle
{
private:
	GameObject* vehicle;
	btScalar mRotation;
	GameObject* mountedWeapon;//Mounted on top of the car
	GameObject* frontWeapon;
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;
	btGeneric6DofSpring2Constraint* spring1;
	btGeneric6DofSpring2Constraint* spring2;
	btGeneric6DofSpring2Constraint* spring3;
	btGeneric6DofSpring2Constraint* spring4;
	GameObject* vehicleBody1;
	btPoint2PointConstraint* pointJoint;
	//btRaycastVehicle* vehicleBody;
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster* m_vehicleRayCaster;
	btCollisionShape* m_wheelShape;
	SpotLight* spotLight;
	int health;
	bool deadImpulse;
	bool immortal;
	float immortalTimer;
	float respawnTimer;
	float totalRespawnTime;
	float powerUpTimers[(int)PowerUpType::Length];

	float trailTimer = 0.0f;
	float driftForce = 0.0f;

	float timeSinceLastShot;
	float timeSinceLastShot2;
	Weapon weapon;
	Weapon weapon2;
	Weapon mountedWeaponData;
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
	int drivingMode;
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
	bool player = false;


	Vector2 curDir;
	Vector2 currentDir;
	float velocitySimple;
	float velocitySpeed;
	class Physics* physics;
public:
	Vehicle();
	virtual ~Vehicle();

	void init(Physics* physics);
	void updatePlayer(float deltaTime);
	void update(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput);
	void updateWeapon(float deltaTime);

	GameObject* getVehicle() { return this->vehicle; }
	GameObject* getVehicleBody1() { return this->vehicleBody1; }
	float getAcceleratorX();

	void setSpotLight(SpotLight* theLight);
	void setDrivingMode(int i);
	bool getDrivingMode();
	Vector3 getVelocity();
	float getVelocitySpeed();
	float getRotator();
	Vector3 getCameraDistance(float deltaTime);
	void setAccelForce(Vector3 accelForce, float deltaTime);
	void setWheelRotation();
	//void onCollision(Vector2 direction);

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(int health);
	void setMaxHealth(int maxHealth);
	void resetHealth();
	void changeHealth(int amount);
	bool isDead() const;
	float getTotalRespawnTime()const;
	float getRespawnTimer()const;
	void makePlayer();
	bool isPlayer() const;

	float getPitch(DirectX::XMVECTOR Quaternion);
	float getYaw(DirectX::XMVECTOR Quaternion);
	float getRoll(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);

	Bullet* getBulletArray(size_t& count);
	void addPowerUp(PowerUpType p);
	void updatePowerUpEffects(float deltaTime);
	float getPowerUpTimer(PowerUpType p);
};

#endif // !VEHICLE_H