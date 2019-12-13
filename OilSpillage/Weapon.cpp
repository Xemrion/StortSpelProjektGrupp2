#include "Weapon.h"
#include "game.h"
#include "States/PlayingGameState.h"

using namespace DirectX::SimpleMath;
float Bullet::soundTimer = 0;
Bullet::Bullet()
{
	this->obj = new GameObject;
	this->obj->setSunShadow(false);
	this->obj->setSpotShadow(false);
	this->obj->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->obj->setScale(Vector3(0.05f, 0.25f, 0.35f));
	this->obj->setColor(Vector4(1.2f, 1.2f, 0, 1));
	this->weapon.type = WeaponType::None;
	this->gadget = nullptr;
}

Bullet::Bullet(Weapon weapon)
{
	this->obj = new GameObject;
	this->obj->setSunShadow(false);
	this->obj->setSpotShadow(false);
	this->obj->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->weapon = weapon;
	this->obj->setScale(weapon.bulletScale);
	this->obj->setColor(Vector4(0, 0, 0, 1));
	this->gadget = nullptr;
}

Bullet::~Bullet()
{
	Game::getGraphics().removeFromDraw(this->obj);
	delete obj;
}

void Bullet::shoot(Weapon& weapon, const Vector3& position, const Vector3& normalizedDir, const Vector3& additionalVelocity, float deltaTime)
{
	this->initPos = position;
	this->initDir = normalizedDir;

	this->weapon = Weapon(weapon);
	if (this->weapon.type == WeaponType::None)
	{

	}
	else if (this->weapon.type == WeaponType::Flamethrower or
		this->weapon.type == WeaponType::aiFlamethrower or
		this->weapon.type == WeaponType::aiBossFlamethrower or
		this->weapon.type == WeaponType::aiBossFlamethrowerPhase2)
	{
		flamethrowerShoot(position, normalizedDir, additionalVelocity);
	}
	else if (this->weapon.type == WeaponType::Laser or this->weapon.type == WeaponType::aiLaser)
	{
		laserShoot(weapon, normalizedDir, deltaTime);
	}
	else if (weapon.type == WeaponType::aiMelee)
	{
		meleeShoot(position, normalizedDir);
	}
	else if (weapon.type == WeaponType::aiMachineGun or weapon.type == WeaponType::aiMissileLauncher or weapon.type == WeaponType::aiBossMachineGun or weapon.type == WeaponType::aiBossMachineGunPhase2 or weapon.type == WeaponType::aiBossMissileLauncher)
	{
		defaultShootEnemy(weapon, position, normalizedDir, additionalVelocity, deltaTime);
	}
	else
	{
		defaultShoot(weapon, position, normalizedDir, additionalVelocity, deltaTime);
	}
}

void Bullet::defaultShoot(Weapon& vehicleWeapon, const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity, float deltaTime)
{
	this->dir = direction;

	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * (weapon.spreadRadians + weapon.currentSpreadIncrease) * 0.5f;
	vehicleWeapon.currentSpreadIncrease = min(vehicleWeapon.currentSpreadIncrease + vehicleWeapon.spreadIncreasePerSecond * vehicleWeapon.maxSpread * deltaTime, vehicleWeapon.maxSpread);
	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);
	this->dir *= weapon.bulletSpeed;

	this->dir += additionalVelocity;
	this->timeLeft = weapon.bulletLifetime;
	this->obj->setPosition(position);
	this->obj->setColor(Vector4(1.2f, 1.2f, 0, 1));

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));
	if (!vehicleWeapon.melee)
		Game::getGraphics().addToDraw(this->obj);
}

void Bullet::defaultShootEnemy(Weapon& vehicleWeapon, const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity, float deltaTime)
{
	this->dir = direction;

	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * (weapon.spreadRadians + weapon.currentSpreadIncrease) * 0.5f;
	vehicleWeapon.currentSpreadIncrease = min(vehicleWeapon.currentSpreadIncrease + vehicleWeapon.spreadIncreasePerSecond * vehicleWeapon.maxSpread * deltaTime, vehicleWeapon.maxSpread);
	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);
	this->dir *= weapon.bulletSpeed;

	this->dir += additionalVelocity;
	this->timeLeft = weapon.bulletLifetime;
	this->obj->setPosition(position);
	this->obj->setColor(Vector4(1.0f, 0.0f, 0.0f, 1));

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));
	if (!vehicleWeapon.melee)
		Game::getGraphics().addToDraw(this->obj);
}

void Bullet::meleeShoot(const Vector3& position, const Vector3& direction)
{
	this->obj->setPosition(position + direction);
}

void Bullet::flamethrowerShoot(const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity)
{
	this->dir = direction;

	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * weapon.spreadRadians * 0.5f;
	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);
	this->dir *= weapon.bulletSpeed;

	this->dir += additionalVelocity;
	this->timeLeft = weapon.bulletLifetime;
	this->obj->setPosition(position);

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));

	this->dir.y = 0.0f;

	for (int i = 0; i < 8; i++)
	{
		Game::getGraphics().addParticle(obj->getPosition() + Vector3(0, 1, 0),
			this->dir,
			1,
			weapon.bulletLifetime,
			0.25f);
	}
}

void Bullet::update(float deltaTime)
{
	if (this->weapon.type == WeaponType::Laser || this->weapon.melee)
	{
		this->obj->setPosition(initPos);
		this->dir = initDir;
	}
	if(this->weapon.type == WeaponType::None)
	{

	}
	else if (this->weapon.type == WeaponType::aiMachineGun or
		this->weapon.type == WeaponType::aiFlamethrower or
		this->weapon.type == WeaponType::aiMissileLauncher or
		this->weapon.type == WeaponType::aiBossFlamethrower or
		this->weapon.type == WeaponType::aiBossFlamethrowerPhase2 or
		this->weapon.type == WeaponType::aiBossMachineGun or
		this->weapon.type == WeaponType::aiBossMachineGunPhase2 or
		this->weapon.type == WeaponType::aiBossMissileLauncher)
	{
		defaultEnemyUpdate(deltaTime);
	}
	else if (this->weapon.type == WeaponType::Laser)
	{
		laserUpdate(deltaTime);
	}
	else if (this->weapon.type == WeaponType::aiLaser)
	{
		laserEnemyUpdate(deltaTime);
	}
	else if (weapon.type == WeaponType::aiMelee)
	{
		enemyMeleeUpdate();
	}
	else
	{
		defaultUpdate(deltaTime);
	}

	if (timeLeft <= 0 && weapon.type != WeaponType::None)
	{
		destroy();
	}
}

void Bullet::defaultUpdate(float& deltaTime)
{
	this->obj->setScale(weapon.bulletScale);
	if (timeLeft > 0.0f)
	{
		obj->move(dir * min(deltaTime, timeLeft));
		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

void Bullet::defaultEnemyUpdate(float& deltaTime)
{
	if (timeLeft > 0.0f)
	{
		obj->move(dir * min(deltaTime, timeLeft));

		if ((this->obj->getPosition() - static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getPosition()).Length() < 1.5f)
		{
			if (soundTimer > 0.05f) {
				int randomSound = rand() % 3 + 1;
				std::string soundEffect = "CarGlass" + std::to_string(randomSound) + ".wav";
				Sound::play(soundEffect);
				Sound::play("MetalImpact1.wav");
				soundTimer = 0;
			}
			if(weapon.doesDoT)
			{
				static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->setFire();
			}
			static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-weapon.damage);
			this->timeLeft = 0.f;
		}

		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

void Bullet::enemyMeleeUpdate()
{

	if ((this->obj->getPosition() - static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getPosition()).Length() < 1.5f)
	{
		if (soundTimer > 0.05f) {
			int randomSound = rand() % 3 + 1;
			std::string soundEffect = "CarGlass" + std::to_string(randomSound) + ".wav";
			Sound::play(soundEffect);
			Sound::play("MetalImpact1.wav");
			soundTimer = 0;
		}
		static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-weapon.damage);
		this->timeLeft = 0.f;
	}
}

void Bullet::laserEnemyUpdate(float& deltaTime)
{
	GameObject* laserObject = this->getGameObject();
	Vector3 rayDir = this->getDirection();
	Vector3 rayOrigin = laserObject->getPosition() - rayDir * laserObject->getScale().z;
	if (static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getAABB().intersect(rayOrigin, rayDir, 1000))
	{
		if (soundTimer > 0.05f)
		{
			int randomSound = rand() % 6 + 1;
			std::string soundEffect = "RobotBullet" + std::to_string(randomSound) + ".mp3";
			Sound::play(soundEffect);
			soundTimer = 0;
		}
		static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-this->getDamage() * deltaTime);
	}
	this->obj->setScale(weapon.bulletScale * Vector3(timeLeft / weapon.bulletLifetime, timeLeft / weapon.bulletLifetime, 1.0));
	if (timeLeft > 0.0f)
	{
		obj->move(dir * this->obj->getScale().z + dir);
		float newRot = atan2(dir.x, dir.z);
		this->obj->setRotation(Vector3(0, newRot, 0));
		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

void Bullet::laserShoot(Weapon& vehicleWeapon, const Vector3& direction, float deltaTime)
{
	if (vehicleWeapon.remainingCooldown > 0.0)
	{
		this->weapon.type == WeaponType::None;
		return;
	}
	this->dir = direction;
	vehicleWeapon.currentSpreadIncrease += vehicleWeapon.spreadIncreasePerSecond * vehicleWeapon.maxSpread * deltaTime;

	this->timeLeft = weapon.bulletLifetime;
	this->obj->setScale(weapon.bulletScale);
	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));

	if (vehicleWeapon.currentSpreadIncrease > vehicleWeapon.maxSpread)
	{
		vehicleWeapon.remainingCooldown = 4.0;
		vehicleWeapon.currentSpreadIncrease = 0.0;
	}
}

void Bullet::laserUpdate(float& deltaTime)
{
	this->obj->setScale(weapon.bulletScale * Vector3(timeLeft / weapon.bulletLifetime, timeLeft / weapon.bulletLifetime, 1.0));
	if (timeLeft > 0.0f)
	{
		obj->move(dir * this->obj->getScale().z + dir);
		float newRot = atan2(dir.x, dir.z);
		this->obj->setRotation(Vector3(0, newRot, 0));
		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

float Bullet::getTimeLeft() const
{
	return timeLeft;
}

void Bullet::setWeapon(Weapon weapon)
{
	this->weapon = weapon;
}

Weapon Bullet::getWeapon() const
{
	return this->weapon;
}

WeaponType Bullet::getWeaponType() const
{
	return this->weapon.type;
}

float Bullet::getDamage() const
{
	return weapon.damage;
}

bool Bullet::getMelee() const
{
	return weapon.melee;
}

bool Bullet::getFlame() const
{
	return weapon.doesDoT;
}

float Bullet::getFlameTimer() const
{
	return weapon.doTTimer;
}

bool Bullet::getKnockback() const
{
	return weapon.doesKnockBack;
}

float Bullet::getKnockbackForce() const
{
	return weapon.knockbackForce;
}

bool Bullet::getSplashBool() const
{
	return weapon.doesSplashDmg;
}

float Bullet::getSplashRange() const
{
	return weapon.splashRange;
}

GameObject* Bullet::getGameObject()
{
	return obj;
}

void Bullet::updateSoundTimer(float deltaTime)
{
	soundTimer += deltaTime;
}

void Bullet::destroy()
{
	setWeapon(WeaponHandler::getWeapon(WeaponType::None));
	Game::getGraphics().removeFromDraw(obj);
}

Vector3 Bullet::getDirection() const
{
	return dir;
}

void Bullet::setDirection(Vector3 newDir)
{
	dir = newDir;
}

Vector3& Bullet::getPosition()
{
	return this->initPos;
}

Gadget Bullet::getGadger() const
{
	return *this->gadget;
}

void Bullet::shootEmp(Gadget& gadget, const Vector3& pos)
{
	this->dir = Vector3();
	this->initPos = pos;
	this->initDir = Vector3();
	this->gadget = &gadget;
	this->timeLeft = gadget.lifeTime;
	this->weapon.type = WeaponType::gadget;
}
