#include "Powerup.h"
#include "game.h"

PowerUp::PowerUp()
{
	init(PowerUpType::Time);
}

PowerUp::PowerUp(Vector3 position, PowerUpType type)
{
	init(type);
	this->setPosition(position);
}

PowerUp::~PowerUp()
{
	Game::getGraphics().removeFromDraw(this);
}

void PowerUp::init(PowerUpType type)
{
	this->type = type;
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	this->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//this->setTexture(Game::getGraphics().getTexturePointer("brickwall"));
	this->setScale(Vector3(1.5, 1.5, 1.5));
}

void PowerUp::update(float time)
{
	this->rotation = Vector3(sin(time), cos(time * 0.1 + 1.0), cos(time));
	this->setColor(Vector4(fmod(0.2 + time, 1.0), fmod(0.63 + time * 1.33, 1.0), fmod(time * 0.81, 1.0), 1.0));
	time *= 3.0;
	this->setScale(Vector3(sin(time) * 0.25 + 1.25, sin(time) * 0.25 + 1.25, sin(time) * 0.25 + 1.25));
}

PowerUpType PowerUp::getPowerUpType()
{
	return type;
}
