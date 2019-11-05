#include "../States/PlayingGameState.h"
#include "Swarm.h"

Swarm::Swarm()
{

}

Swarm::Swarm(float x, float z, int weaponType)
	: Actor(x, z, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));

	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();

	this->defaultStats = VehicleStats::AISwarm;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
	Game::getGraphics().loadModel("Entities/Dummy_Drone");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Drone");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Drone"));

	boidOffset = 2.5;
}

Swarm::~Swarm()
{

}

void Swarm::update(float dt, Vector3 targetPos)
{
	this->targetPos = targetPos;
	//updateWeapon(dt);
	//this->root->func();
	followPath();
}

void Swarm::setUpActor()
{

}
