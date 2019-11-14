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
}

Bullet::~Bullet()
{
	Game::getGraphics().removeFromDraw(this->obj);
	delete obj;
}

void Bullet::shoot(Weapon& weapon, Vector3 position, Vector3 direction, Vector3 additionalVelocity, float deltaTime)
{
	this->weapon = Weapon(weapon);
	if (this->weapon.type == WeaponType::None)
	{

	}
	else if (this->weapon.type == WeaponType::Flamethrower or this->weapon.type == WeaponType::aiFlamethrower)
	{
		flamethrowerShoot(weapon, position, direction, additionalVelocity, deltaTime);
	}
	else if (this->weapon.type == WeaponType::Laser or this->weapon.type == WeaponType::aiLaser)
	{
		laserShoot(weapon, position, direction, additionalVelocity, deltaTime);
	}
	else
	{
		defaultShoot(weapon, position, direction, additionalVelocity, deltaTime);
	}
}

void Bullet::defaultShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime)
{
	direction.Normalize();
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

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));

	Game::getGraphics().addToDraw(this->obj);
}

void Bullet::flamethrowerShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime)
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

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));

	//Game::getGraphics().addToDraw(this->obj);
	this->dir.y = 0.0f;

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
	if (this->weapon.type == WeaponType::None)
	{

	}
	else if (this->weapon.type == WeaponType::aiMachineGun or
		this->weapon.type == WeaponType::aiFlamethrower or
		this->weapon.type == WeaponType::aiMelee or
		this->weapon.type == WeaponType::aiMissileLauncher)
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
	else
	{
		defaultUpdate(deltaTime);
	}

	if (timeLeft == 0)
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

		if ((this->obj->getPosition() - static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getPosition()).Length() < 1.5f)
		{
			if (soundTimer > 0.05f) {
				int randomSound = rand() % 3 + 1;
				std::wstring soundEffect = L"data/sound/CarGlass" + std::to_wstring(randomSound) + L".wav";
				Sound::PlaySoundEffect(soundEffect);
				Sound::PlaySoundEffect(L"data/sound/MetalImpact1.wav");
				soundTimer = 0;
			}
			static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-weapon.damage);
			this->timeLeft = 0.f;
		}

		timeLeft = max(timeLeft - deltaTime, 0.0f);
	}
}

void Bullet::laserEnemyUpdate(float& deltaTime)
{
	GameObject* laserObject = this->getGameObject();
	Vector3 rayDir = this->getDirection();
	Vector3 rayOrigin = laserObject->getPosition() - rayDir * laserObject->getScale().z;
	if (static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getAABB().intersect(rayOrigin, rayDir, 1000))
	{
		if (soundTimer > 0.05f) {
			/*int randomSound = rand() % 3 + 1;
			std::wstring soundEffect = L"data/sound/MetalImpactPitched" + to_wstring(randomSound) + L".wav";
			Sound::PlaySoundEffect(soundEffect);*/
			Sound::PlaySoundEffect(L"data/sound/HitSound.wav");
			soundTimer = 0;
		}
		static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-this->getDamage());
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

void Bullet::laserShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime)
{
	if (vehicleWeapon.remainingCooldown > 0.0)
	{
		this->weapon.type == WeaponType::None;
		return;
	}
	direction.Normalize();
	this->dir = direction;
	float randomNumber = (float(rand()) / (float(RAND_MAX) * 0.5f)) - 1.0f;
	float spread = randomNumber * (weapon.spreadRadians + weapon.currentSpreadIncrease) * 0.5f;
	vehicleWeapon.currentSpreadIncrease += vehicleWeapon.spreadIncreasePerSecond * vehicleWeapon.maxSpread * deltaTime;

	this->dir.x = direction.x * cos(spread) - direction.z * sin(spread);
	this->dir.z = direction.x * sin(spread) + direction.z * cos(spread);

	this->timeLeft = weapon.bulletLifetime;
	this->obj->setScale(weapon.bulletScale);
	this->obj->setPosition(position + direction * this->obj->getScale().z);

	float newRot = atan2(direction.x, direction.z);
	this->obj->setRotation(Vector3(0, newRot, 0));
	this->obj->setColor(Vector4::Lerp(Vector4(0.5, 1.0, 4.5, 0.2), Vector4(4.5, 0.5, 0.5, 0.02), (vehicleWeapon.currentSpreadIncrease * vehicleWeapon.currentSpreadIncrease) / (vehicleWeapon.maxSpread * vehicleWeapon.maxSpread)));

	Game::getGraphics().addToDraw(this->obj);
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

int Bullet::getDamage() const
{
	return weapon.damage;
}

GameObject* Bullet::getGameObject()
{
	return obj;
}

void Bullet::updateSoundTimer(float deltaTime)
{
	soundTimer +=  deltaTime;
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
