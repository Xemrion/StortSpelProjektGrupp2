#include "Weakspot.h"

Weakspot::Weakspot()
{
	this->weakspotNr = 0;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;
	this->maxHP = this->stats.maxHealth;
	this->isHit = 1;
}

Weakspot::Weakspot(Weakspot&& weakspot) //move constructor for ex. vectors
{
	this->weakspotNr = weakspot.weakspotNr;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;
	this->maxHP = this->stats.maxHealth;
	this->isHit = weakspot.isHit;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Shield");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Shield");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Shield"));

	Game::getGraphics().addToDraw(this);
}

Weakspot::Weakspot(int weaponType)
{
	this->weakspotNr = 0;
	this->stats = VehicleStats::AIPart;
	this->health = this->stats.maxHealth;
	this->maxHP = this->stats.maxHealth;
	this->isHit = 1;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Shield");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Shield");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Shield"));

	Game::getGraphics().addToDraw(this);
}

Weakspot::Weakspot(const Weakspot& other)	//copy constructor
{
	this->weakspotNr = other.weakspotNr;
	this->stats = other.stats;
	this->health = other.health;
	this->maxHP = other.maxHP;
	this->isHit = other.isHit;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));

	Game::getGraphics().loadModel("Entities/Shield");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Shield");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Shield"));

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
		this->maxHP = other.maxHP;
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

void Weakspot::changeHealth(int damage, float dt)
{
	isHit = true; //start lowering redness

	this->setColor(Vector4(max((this->getColor().x + (damage * 0.1f)), 0), this->getColor().y, this->getColor().z, 1)); //makes it really red
	Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 1, 0.5f);
	this->health -= damage;
}

void Weakspot::setWeakspotNr(int weakspotNr)
{
	this->weakspotNr = weakspotNr;
}

void Weakspot::setIsHit(bool isHit)
{
	this->isHit = isHit;
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

const int Weakspot::getMaxHP()
{
	return this->maxHP;
}

const bool Weakspot::getIsHit()
{
	return this->isHit;
}

//Problem
//kan den bli skadad av skott? hur ändras health?
//boss vector i actormanager
//function i boss som kallas i actor manager