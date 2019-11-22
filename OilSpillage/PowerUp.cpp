#include "Powerup.h"
#include "game.h"

PowerUp::PowerUp()
{
	this->type = PowerUpType::Time;
	this->setPosition(Vector3(0.0, 0.0, 0.0));
	this->currentRespawnTimer = 10000000.0;
	this->respawnTime = currentRespawnTimer;
	this->time = 0.0;
}

PowerUp::PowerUp(Vector3 position, PowerUpType type, float respawnTime)
{
	this->type = type;
	this->setPosition(position);
	this->currentRespawnTimer = 10000000.0;
	this->respawnTime = respawnTime;
	this->time = 0.0;
}

PowerUp::PowerUp(const PowerUp& p)
{
	this->type = p.type;
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->setPosition(p.position);
	this->currentRespawnTimer = p.currentRespawnTimer;
	this->respawnTime = p.respawnTime;
	this->time = 0.0;
}

PowerUp::PowerUp(PowerUp&& p) noexcept
{
	this->type = p.type;
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->setPosition(p.position);
	this->currentRespawnTimer = p.currentRespawnTimer;
	this->respawnTime = p.respawnTime;
	this->time = 0.0;
}

PowerUp& PowerUp::operator=(const PowerUp& p)
{
	this->type = p.type;
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->setPosition(p.position);
	this->currentRespawnTimer = p.currentRespawnTimer;
	this->respawnTime = p.respawnTime;
	this->time = 0.0;

	return *this;
}

PowerUp& PowerUp::operator=(PowerUp&& p) noexcept
{
	this->type = p.type;
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->setPosition(p.position);
	this->currentRespawnTimer = p.currentRespawnTimer;
	this->respawnTime = p.respawnTime;
	this->time = 0.0;

	return *this;
}

PowerUp::~PowerUp()
{
	Game::getGraphics().removeFromDraw(this);
}

void PowerUp::update(float deltaTime)
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
		this->setScale(Vector3(sin(time * 3.0) * 0.05 + 1.25, sin(time * 3.0) * 0.05 + 1.25, sin(time * 3.0) * 0.05 + 1.25));
	}
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
