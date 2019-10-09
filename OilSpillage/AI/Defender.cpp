#include "..//game.h"
#include "Defender.h"


Defender::Defender()
{
	setUpActor();
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

	for (int i = 0; i < bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			this->bullets[i].timeLeft -= deltaTime;
			this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
			Game::getGraphics().addToDraw(this->bullets[i].obj);
		}
	}
}
void Defender::setUpActor()
{
	root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector2 = bt.getSelector();
	Sequence& sequence2 = bt.getSequence();

	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Defender::enemyNear, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Defender::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Defender::setRoamState, std::ref(*this)));
	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Defender::inRange, std::ref(*this)));
	Behavior& attack = bt.getAction();
	attack.addAction(std::bind(&Defender::shoot, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(enemyNear);
	sequence.addChildren(selector2);

	selector2.addChildren(sequence2);
	selector2.addChildren(chase);

	sequence2.addChildren(inRange);
	sequence2.addChildren(attack);
}

Status Defender::inRange()
{
	Status status;
	float tempDelta = deltaTime + this->leftoverTime;

	if ((getPosition() - targetPos).Length() > 7 && tempDelta <= this->weapon.fireSpeed)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}
