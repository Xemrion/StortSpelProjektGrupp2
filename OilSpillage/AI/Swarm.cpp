#include "../States/PlayingGameState.h"
#include "Swarm.h"

Swarm::Swarm()
{

}

Swarm::Swarm(float x, float z, int weaponType)
	: DynamicActor(x,z),Melee()
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();

	this->stats = VehicleStats::AISwarm;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Drone");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Drone");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Drone"));
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);
	Game::getGraphics().addToDraw(this);

	boidOffset = 2.5;
}

Swarm::~Swarm()
{

}

void Swarm::setUpActor()
{

}