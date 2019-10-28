#include "Weapon.h"
#include "game.h"
#include "States/PlayingGameState.h"

using namespace DirectX::SimpleMath;

Bullet::Bullet()
{
	this->obj = new GameObject;
	this->obj->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->obj->setScale(Vector3(0.25f, 0.25f, 0.25f));
	this->obj->setColor(Vector4(1, 1, 0, 1));
}

Bullet::Bullet(WeaponType type)
{
	this->obj = new GameObject;
	this->obj->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->obj->setScale(WeaponHandler::weapons[(int)this->weaponType].bulletScale);
	this->obj->setColor(Vector4(1, 1, 0, 1));
	this->weaponType = type;
}

Bullet::~Bullet()
{
	Game::getGraphics().removeFromDraw(this->obj);
	delete obj;
}

void Bullet::shoot(Weapon& weapon, Vector3 position, Vector3 direction, Vector3 additionalVelocity)
{
	this->weaponType = weapon.type;
	if (this->weaponType == WeaponType::None)
	{

	}
	else if (this->weaponType == WeaponType::Flamethrower)
	{
		flamethrowerShoot(weapon, position, direction, additionalVelocity);
	}
	else
	{
		defaultShoot(weapon, position, direction, additionalVelocity);
	}
}

void Bullet::defaultShoot(Weapon& weapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity)
{
	direction.Normalize();
	this->dir = direction;

	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * (weapon.spreadRadians + weapon.currentSpreadIncrease) * 0.5f;
	weapon.currentSpreadIncrease = min(weapon.currentSpreadIncrease + weapon.spreadIncreasePerShot, weapon.maxSpread);
	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);
	this->dir *= weapon.bulletSpeed;

	this->dir += additionalVelocity;
	this->timeLeft = weapon.bulletLifetime;
	this->obj->setPosition(position);
	this->obj->setRotation(Vector3(XMVector3AngleBetweenVectors(Vector3(0, 0, 1), this->dir)) * Vector3(0, 1, 0));
	Game::getGraphics().addToDraw(this->obj);
}

void Bullet::flamethrowerShoot(Weapon& weapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity)
{
	direction.Normalize();
	this->dir = direction;

	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * weapon.spreadRadians * 0.5f;
	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);
	this->dir *= weapon.bulletSpeed;

	this->dir += additionalVelocity;
	this->timeLeft = weapon.bulletLifetime;
	this->obj->setPosition(position);
	this->obj->setRotation(Vector3(XMVector3AngleBetweenVectors(Vector3(0, 0, 1), this->dir)) * Vector3(0, 1, 0));

#ifdef _DEBUG
	Game::getGraphics().addToDraw(this->obj);
#endif

	Game::getGraphics().addParticle(obj->getPosition() + Vector3(0, 1, 0),
		this->dir,
		1,
		weapon.bulletLifetime + 0.5f,
		0.25f);
	Game::getGraphics().addParticle(obj->getPosition()+Vector3(0,1,0),
		this->dir,
		1,
		weapon.bulletLifetime + 0.5f,
		0.25f);
	Game::getGraphics().addParticle(obj->getPosition() + Vector3(0, 1, 0),
		this->dir,
		1,
		weapon.bulletLifetime + 0.5f,
		0.25f);
	Game::getGraphics().addParticle(obj->getPosition() + Vector3(0, 1, 0),
		this->dir,
		1,
		weapon.bulletLifetime + 0.5f,
		0.25f);
}

void Bullet::update(float deltaTime)
{
	if (this->weaponType == WeaponType::None)
	{

	}
	else if (this->weaponType == WeaponType::aiMachineGun)
	{
		defaultEnemyUpdate(deltaTime);
	}
	else
	{
		defaultUpdate(deltaTime);
	}

	if (timeLeft == 0.0)
	{
		this->weaponType = WeaponType::None;
	}
}

void Bullet::defaultUpdate(float& deltaTime)
{
	if (timeLeft > 0.0f)
	{
		obj->move(dir * min(deltaTime, timeLeft));

		if (timeLeft < deltaTime)
		{
			Game::getGraphics().removeFromDraw(this->obj);
		}

		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

void Bullet::defaultEnemyUpdate(float& deltaTime)
{
	if (timeLeft > 0.0f)
	{
		obj->move(dir * min(deltaTime, timeLeft));

		if ((this->obj->getPosition() - static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getPosition()).Length() < 1.5f)
		{
		
			static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-WeaponHandler::weapons[(int)weaponType].damage);
			this->timeLeft = 0.f;
		}

		if (timeLeft < deltaTime)
		{
			Game::getGraphics().removeFromDraw(this->obj);
		}

		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

float Bullet::getTimeLeft() const
{
	return timeLeft;
}

void Bullet::setWeaponType(WeaponType type) 
{
	this->weaponType = type;
}

WeaponType Bullet::getWeaponType() const
{
	return this->weaponType;
}

int Bullet::getDamage() const
{
	return WeaponHandler::weapons[(int)this->weaponType].damage;
}

GameObject* Bullet::getGameObject()
{
	return obj;
}
