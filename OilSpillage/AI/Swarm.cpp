#include "../States/PlayingGameState.h"
#include "Swarm.h"

Swarm::Swarm()
{

}

Swarm::Swarm(float x, float z, int weaponType)
	: Actor(x, z, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();

	this->defaultStats = VehicleStats::AISwarm;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
	Game::getGraphics().loadModel("Entities/Drone");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Drone");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Drone"));
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);
	Game::getGraphics().addToDraw(this);

	boidOffset = 2.5;
	this->state = State::Chasing;
}

Swarm::~Swarm()
{

}

void Swarm::update(float dt, Vector3 targetPos)
{
	this->targetPos = targetPos;
	this->deltaTime = dt;
	updateBullets(dt);
	//this->root->func();
	followPath();
	shoot();
}

void Swarm::setUpActor()
{
}

Status Swarm::shoot()
{
	if ((position - targetPos).Length() < 23)
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

			if (bullets[0].getTimeLeft() == 0.0)
			{
				Vector3 dir = (targetPos - this->position);
				dir.Normalize();
				Vector3 bulletOrigin = this->position + dir;
				dir = (targetPos - bulletOrigin);
				dir.Normalize();

				this->bullets[0].setWeaponType(this->weapon.type);
				this->bullets[0].shoot(
					weapon,
					bulletOrigin,
					dir,
					this->velocity
				);
			}
			
		}
	}

	return Status::SUCCESS;
}
