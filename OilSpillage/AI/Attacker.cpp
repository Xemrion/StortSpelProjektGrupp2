#include "../States/PlayingGameState.h"
#include "Attacker.h"

Attacker::Attacker()
{
	setUpActor();

	this->defaultStats = VehicleStats::AIAttacker;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Attacker::Attacker(float x, float z, AStar* aStar)
	:Actor(x, z, aStar)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	setUpActor();

	this->defaultStats = VehicleStats::AIAttacker;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
	Game::getGraphics().loadModel("Entities/Dummy_Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Roller_Melee"));

	weapon = AIWeapon::machineGun;
}

Attacker::~Attacker()
{
}

void Attacker::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;

	if (nrOfFrames % 100 == 0)
	{
		//findPath();
		nrOfFrames = 1;
	}

	this->root->func();
	followPath();
	this->rotateTowards();

	for (int i = 0; i < bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			if ((this->bullets[i].obj->getPosition() - this->targetPos).Length() < 0.5f)
			{
				static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-20);
				this->bullets[i].timeLeft = 0;
			}
			else
			{
				this->bullets[i].timeLeft -= deltaTime;
				this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
				Game::getGraphics().addToDraw(this->bullets[i].obj);
			}
		}
	}
	nrOfFrames++;
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inRange, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Attacker::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Attacker::setRoamState, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Attacker::shoot, std::ref(*this)));
	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Attacker::enemyNear, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(enemyNear);
	sequence.addChildren(selector);
	
	selector.addChildren(seq2);
	selector.addChildren(chase);

	seq2.addChildren(inRange);
	seq2.addChildren(shoot);
}

void Attacker::rotateTowards()
{
	if(this->enemyNear() == Status::SUCCESS)
	{
		Vector3 targetToSelf = (targetPos - position);
		//targetToSelf.Normalize();
		if ((targetToSelf).Dot(vecForward) < 0.8)
		{
			vecForward -= (targetToSelf * deltaTime) / 0.1;
			vecForward.Normalize();

			float newRot = atan2(this->vecForward.x, this->vecForward.z);
			this->setRotation(Vector3(0, newRot - (XM_PI / 2), 0));
		}
	}



}