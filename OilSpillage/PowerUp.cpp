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
	this->setTexture(Game::getGraphics().getTexturePointer("brickwall"));
	this->setScale(Vector3(0.5, 0.5, 0.5));
}

void PowerUp::update(float time)
{
	this->rotation = Vector3(sin(time), cos(time * 0.1 + 1.0), cos(time));
}

PowerUpType PowerUp::getPowerUpType()
{
	return type;
}
