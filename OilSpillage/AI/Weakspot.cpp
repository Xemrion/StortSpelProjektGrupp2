#include "Weakspot.h"

Weakspot::Weakspot()
{

}

Weakspot::Weakspot(int weaponType, Stats stats):stats(stats)
{
	this->dead = 0;
	this->weakspotNr = 0;

	this->setScale(Vector3(1.0f, 1.0f, 1.0f));
	this->setUpActor();
	Game::getGraphics().addToDraw(this);

	//this->stats = VehicleStats::AIPart;
	//setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Barrel");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Barrel"));
}

Weakspot::~Weakspot()
{
	Game::getGraphics().removeFromDraw(this);
}

void Weakspot::update(float dt, btVector3 velocityVec)
{
	//Calls this update and not place, where, how?
	//this->move(velocityVec);
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

void Weakspot::updateSelf()
{
	//this->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
}

void Weakspot::setUpActor()
{
	
}

void Weakspot::move(btVector3 velocityVec) //not used
{
	//this->getRigidBody()->setLinearVelocity(velocityVec);
}

void Weakspot::setWeakspotNr(int weakspotNr)
{
	this->weakspotNr = weakspotNr;
}

const bool Weakspot::getDead()
{
	if (this->stats.maxHealth <= 0)
		return 1;
	else
		return 0;
}

const int Weakspot::getWeakspotNr()
{
	return this->weakspotNr;
}

//Probem
//kan den bli skadad av skott? hur ändras health?
//Spawnar inte: 0 hp, drawn?