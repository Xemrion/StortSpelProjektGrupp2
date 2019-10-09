#include "..//game.h"
#include "Defender.h"


Defender::Defender()
{
	setUpActor();
	objectivePos = Vector3(9.0f,0.0f,9.0f);
}

Defender::Defender(float x, float z, AStar* aStar)
	:Actor(x, z, aStar)
{
	this->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	setUpActor();
	objectivePos = Vector3(9.0f, 0.0f, 9.0f);
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
	Selector& selector3 = bt.getSelector();
	Sequence& sequence3 = bt.getSequence();

	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Defender::enemyWhithinObjective, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Defender::setChaseState, std::ref(*this)));
	//Behavior& roam = bt.getAction();
	//roam.addAction(std::bind(&Defender::setRoamState, std::ref(*this)));
	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Defender::inRange, std::ref(*this)));
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

void Defender::followPath()
{
	if (path.size() > 0)
	{
		targetNode = DirectX::SimpleMath::Vector3(float(path.at(path.size() - 1)->GetXPos()),
			.0f,
			float(path.at(path.size() - 1)->GetYPos()));
		Vector3 dir = targetNode - this->getPosition();
		dir.Normalize();
		Vector3 newPosition = this->getPosition() + dir * deltaTime;

		if (newPosition.Distance(targetNode, newPosition) < 1)
		{
			path.pop_back();
		}
		//this->setPosition(newPosition);
	}
	else
	{
		if (this->state == State::Chasing)
		{
			chase();
		}
		else if (this->state == State::Roaming)
		{
			roam();
		}
		else if (this->state == State::Returning)
		{
			returning();
		}
	}
	destination = targetNode;
}

void Defender::returning()
{
	targetPos = objectivePos;
	findPath();
}

Status Defender::inRange()
{
	Status status;
	float tempDelta = deltaTime + this->leftoverTime;

	if ((this->getPosition() - targetPos).Length() > 7 && tempDelta <= this->weapon.fireSpeed)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Defender::inObjectiveRange()
{
	Status status;

	if ((getPosition() - objectivePos).Length() > 4)
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
	return Status::SUCCESS;
}

Status Defender::enemyWhithinObjective()
{
	Status status;

	if ((targetPos - objectivePos).Length() > 12)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}
