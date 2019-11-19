#include "Weakspot.h"

Weakspot::Weakspot()
{
	this->weakspotNr = 0;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;
}

Weakspot::Weakspot(Weakspot&& weakspot) //move constructor for ex. vectors
{
	this->weakspotNr = weakspot.weakspotNr;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Barrel");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Barrel"));

	Game::getGraphics().addToDraw(this);
}

Weakspot::Weakspot(int weaponType)
{
	this->weakspotNr = 0;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Barrel");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Barrel"));

	Game::getGraphics().addToDraw(this);
}

Weakspot::Weakspot(const Weakspot& other)	//copy constructor
{
	this->weakspotNr = other.weakspotNr;
	this->stats = other.stats;
	this->health = other.health;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Barrel");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Barrel"));

	Game::getGraphics().addToDraw(this);
}

Weakspot::~Weakspot()
{
	Game::getGraphics().removeFromDraw(this);
}

Weakspot& Weakspot::operator=(const Weakspot& other)	//equal operator
{
	if (this != &other)
	{
		this->weakspotNr = other.weakspotNr;
		this->stats = other.stats;
		this->health = other.health;
	}
	return *this;
}

void Weakspot::startPos(Vector3 startPos)
{
	this->setPosition(startPos);
}

void Weakspot::shortMove(Vector3 posVec)
{
	Vector3 newPos;
	newPos.x = this->getPosition().x + posVec.x;
	newPos.y = this->getPosition().y + posVec.y;
	newPos.z = this->getPosition().z + posVec.z;

	this->setPosition(newPos);
}

void Weakspot::changeHealth(int damage)
{
	this->health -= damage;
}

void Weakspot::setWeakspotNr(int weakspotNr)
{
	this->weakspotNr = weakspotNr;
}

const bool Weakspot::getDead()
{
	if (this->health <= 0)
		return 1;
	else
		return 0;
}

const int Weakspot::getWeakspotNr()
{
	return this->weakspotNr;
}

const int Weakspot::getHealth()
{
	return this->health;
}

//Problem
//kan den bli skadad av skott? hur ändras health?
//boss vector i actormanager
//function i boss som kallas i actor manager