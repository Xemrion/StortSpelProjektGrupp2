#include "../States/PlayingGameState.h"
#include "Defender.h"


Defender::Defender()
{
	setUpActor();
	objectivePos = Vector3(9.0f, 0.0f, 9.0f);

	this->defaultStats = VehicleStats::AIDefender;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Defender::Defender(float x, float z, AStar* aStar, Vector3 objectivePos)
	:Actor(x, z, aStar)
{
	this->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	setUpActor();
	this->objectivePos = objectivePos;

	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);

	this->defaultStats = VehicleStats::AIDefender;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Defender::~Defender()
{

}
void Defender::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;

	this->root->func();
	followPath();

	if (nrOfFrames % 60 == 0)
	{
		if (this->state == State::Returning && inObjectiveRange() == Status::SUCCESS)
		{
			setRoamState();
		}
		nrOfFrames = 0;
	}

	nrOfFrames++;
}

void Defender::setUpActor()
{
	root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector2 = bt.getSelector();
	Sequence& sequence2 = bt.getSequence();
	Selector& selector3 = bt.getSelector();
	Sequence& sequence3 = bt.getSequence();

	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Defender::enemyWithinObjective, std::ref(*this)));

	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Defender::setChaseState, std::ref(*this)));

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Defender::inAttackRange, std::ref(*this)));

	Behavior& attack = bt.getAction();
	attack.addAction(std::bind(&Defender::shoot, std::ref(*this)));

	Behavior& returning = bt.getAction();
	returning.addAction(std::bind(&Defender::setReturnState, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Defender::idle, std::ref(*this)));

	Behavior& inObjectiveRange = bt.getAction();
	inObjectiveRange.addAction(std::bind(&Defender::inObjectiveRange, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(returning);

	sequence.addChildren(inObjectiveRange);
	sequence.addChildren(selector2);

	selector2.addChildren(sequence2);
	selector2.addChildren(idle);

	sequence2.addChildren(enemyNear);
	sequence2.addChildren(selector3);

	selector3.addChildren(sequence3);
	selector3.addChildren(chase);

	sequence3.addChildren(inRange);
	sequence3.addChildren(attack);
}
void Defender::returning()
{
	targetPos = objectivePos;
	aStar->algorithm(this->getPosition(), targetPos, path);
}

Status Defender::inObjectiveRange()
{
	Status status;

	if ((getPosition() - objectivePos).Length() > 30)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Defender::idle()
{
	return Status::SUCCESS;
}

Status Defender::setReturnState()
{
	this->state = State::Returning;
	returning();
	return Status::SUCCESS;
}

Status Defender::enemyWithinObjective()
{
	Status status;

	if ((targetPos - objectivePos).Length() > 30)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}