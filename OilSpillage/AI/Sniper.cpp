#include "Sniper.h"
#include "..//Input.h"
#include "..//States/PlayingGameState.h"
Sniper::Sniper()
{

}

Sniper::Sniper(float x, float z, int weaponType, Physics* physics)
	:DynamicActor(x, z, physics), Ranged(&this->position, &this->targetPos, &this->velocity, &this->deltaTime, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AISniper;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));
}

Sniper::~Sniper()
{
	Game::getGraphics().removeFromDraw(this);
}

void Sniper::update(float dt, const Vector3& targetPos)
{
	DynamicActor::update(dt, targetPos);
	this->updateBullets(dt);
}

void Sniper::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Sniper::inAttackRange, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Sniper::shoot, std::ref(*this)));

	root->addChildren(sequence);
	sequence.addChildren(inRange);
	sequence.addChildren(shoot);
}

Vector3 Sniper::calculateVelocity()
{
	Vector3 desiredDirection;
	Vector3 offsetVec;
	Vector3 eliminatingVec = Vector3(0.0f, -1.0f, 0.0f) - Vector3(0.0f, 1.0f, 0.0f);
	Vector3 aimRay = Vector3(Input::getDirectionR().x, 0.0f, Input::getDirectionR().y);
	//Ray casting stuff
	if (this->getAABB().intersect(targetPos, aimRay, 1000))
	{
		if (this->stats.speed == 3.0)
		{
			this->stats.speed = 7.0;
		}
		Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
		offsetVec = crossVector.Cross(eliminatingVec);
		offsetVec.Normalize();
		offsetVec *= 8;
		desiredDirection -= position - (position + offsetVec);
	}
	else
	{
		//standard group movement
		if (!vActive)
		{
			desiredDirection -= position - destination;
			//desired *= maxSpeed;
			if (this->stats.speed != 3.0)
			{
				this->stats.speed = 3.0;
			}
		}

		//strafe to the left of player
		else if (vActive)
		{
			Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
			crossVector.Normalize();
			crossVector *= -10;
			desiredDirection -= position - (destination - crossVector);
		}
	}
	vActive = false;
	return  desiredDirection - velocity;
}
