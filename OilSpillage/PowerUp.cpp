#include "Powerup.h"
#include "game.h"

PowerUp::PowerUp()
{
	this->type = PowerUpType::Time;
	this->setPosition(Vector3(0.0, 0.0, 0.0));
	this->currentRespawnTimer = 10000000.0;
	this->respawnTime = currentRespawnTimer;
	this->time = 0.0;
	this->acceleration = 0.0f;
	loadModel();
}

PowerUp::PowerUp(Vector3 position, Physics* physics, PowerUpType type, float respawnTime)
{
	this->type = type;
	this->setPosition(position);
	this->currentRespawnTimer = 10000000.0;
	this->respawnTime = respawnTime;
	this->time = 0.0;
	this->physics = physics;
	this->acceleration = 0.0f;
	this->initRigidBody();
	loadModel();
}

PowerUp::PowerUp(const PowerUp& p)
{
	clone(p);
}

PowerUp::PowerUp(PowerUp&& p) noexcept
{
	clone(p);
}

PowerUp& PowerUp::operator=(const PowerUp& p)
{
	clone(p);

	return *this;
}

PowerUp& PowerUp::operator=(PowerUp&& p) noexcept
{
	clone(p);

	return *this;
}

void PowerUp::clone(const PowerUp& p)
{
	this->type = p.type;
	this->mesh = p.mesh;
	this->material = p.material;
	this->setPosition(p.position);
	this->currentRespawnTimer = p.currentRespawnTimer;
	this->respawnTime = p.respawnTime;
	this->time = 0.0;
	this->physics = p.physics;
	this->acceleration = p.acceleration;
	this->initRigidBody();
}

void PowerUp::loadModel()
{
	if (type == PowerUpType::Health)
	{
		mesh = Game::getGraphics().getMeshPointer("Entities/PowerUps/HealthBoost");
		setMaterial(Game::getGraphics().getMaterial("Entities/PowerUps/HealthBoost"));
	}
	else if (type == PowerUpType::Speed)
	{
		mesh = Game::getGraphics().getMeshPointer("Entities/PowerUps/SpeedBoost");
		setMaterial(Game::getGraphics().getMaterial("Entities/PowerUps/SpeedBoost"));
	}
	else if (type == PowerUpType::Time)
	{
		mesh = Game::getGraphics().getMeshPointer("Entities/PowerUps/TimeBoost");
		setMaterial(Game::getGraphics().getMaterial("Entities/PowerUps/TimeBoost"));
	}
	else if (type == PowerUpType::Star)
	{
		mesh = Game::getGraphics().getMeshPointer("Entities/Star");
		setMaterial(Game::getGraphics().getMaterial("Entities/Star"));
	}
}

void PowerUp::initRigidBody()
{
	btRigidBody* tempo = this->physics->addSphere(0.2f, btVector3(position.x, position.y, position.z), 1.5f, this);
	setRigidBody(tempo, this->physics);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
}

PowerUp::~PowerUp()
{
	Game::getGraphics().removeFromDraw(this);
}

void PowerUp::update(float deltaTime, Vector3 playerPos) 
{
	this->time += deltaTime;
	if (this->currentRespawnTimer > 0.0)
	{
		this->currentRespawnTimer -= deltaTime;
		if (this->currentRespawnTimer <= 0.0)
		{
			activate();
		}
	}
	
	if (isActive())
	{
		this->rotation = Vector3(sin(time), cos(time * 0.1 + 1.0), cos(time));
		this->setColor(Vector4(fmod(0.2 + time, 1.0), fmod(0.63 + time * 1.33, 1.0), fmod(time * 0.81, 1.0), 1.0));
		if (type == PowerUpType::Star)
		{
			this->setScale(Vector3(sin(time * 3.0) * 0.05 + 0.4, sin(time * 3.0) * 0.05 + 0.4, sin(time * 3.0) * 0.05 + 0.4));
		}
		else {
			this->setScale(Vector3(sin(time * 3.0) * 0.05 + 0.8, sin(time * 3.0) * 0.05 + 0.8, sin(time * 3.0) * 0.05 + 0.8));
		}
	}

	Vector3 towardsPlayer = playerPos - this->position;
	towardsPlayer.Normalize(); //so not faster if closer
	float velocityIncrease = 8;
	float accelerationChange = 0.125;
	float distanceFromPlayerToPowerup = 
		  sqrtf((this->position.x - playerPos.x) * (this->position.x - playerPos.x) +
				(this->position.y - playerPos.y) * (this->position.y - playerPos.y) +
				(this->position.z - playerPos.z) * (this->position.z - playerPos.z));
	if (distanceFromPlayerToPowerup <= 10) //do another check for player hp
	{
		this->acceleration += accelerationChange;
	}
	else
	{
		if (this->acceleration > 0)
			this->acceleration -= accelerationChange;
		else if (this->acceleration < 0)
			this->acceleration = 0;
	}
	this->getRigidBody()->setLinearVelocity(btVector3(towardsPlayer.x * velocityIncrease * this->acceleration, 0.0f, towardsPlayer.z * velocityIncrease * this->acceleration));
}

PowerUpType PowerUp::getPowerUpType() const
{
	return type;
}

void PowerUp::deactivate()
{
	Game::getGraphics().removeFromDraw(this);
	currentRespawnTimer = respawnTime;
}

void PowerUp::activate()
{
	Game::getGraphics().addToDraw(this);
	currentRespawnTimer = 0.0;
}

bool PowerUp::isActive() const
{
	return this->currentRespawnTimer == 0.0;
}

float PowerUp::getCurrentRespawnTimer() const
{
	return currentRespawnTimer;
}
