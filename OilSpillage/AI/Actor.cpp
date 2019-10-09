#include "..//game.h"
#include "Actor.h"

Actor::Actor()
{
	this->setUpActor();
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++) {
			boids.push_back(new Boid(float(i), float(j)));
		}

	}
	for (int i = 0; i < boids.size(); i++)
	{
		boids.at(i)->mesh = Game::getGraphics().getMeshPointer("Cube");
		Game::getGraphics().addToDraw(boids.at(i));
		boids.at(i)->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		boids.at(i)->setScale(Vector3(0.5f, 0.5f, 0.5f));
	}
	this->leftoverTime = 0;
	for (int i = 0; i < bulletCount; i++)
	{
		this->bullets[i].obj = new GameObject;
		this->bullets[i].obj->mesh = Game::getGraphics().getMeshPointer("Cube");
		this->bullets[i].obj->setScale(Vector3(0.25f, 0.25f, 0.25f));
		this->bullets[i].obj->setColor(Vector4(1, 1, 0, 1));
	}
}

Actor::~Actor()
{
	//this->root = nullptr;
	for (int i = 0; i < boids.size(); i++)
	{
		delete boids.at(i);
	}
	boids.clear();
	for (int i = 0; i < bulletCount; i++)
	{
		delete this->bullets[i].obj;
	}
}

void Actor::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;

	this->root->func();
	followPath();

	for (int i = 0; i < boids.size(); i++) //Updating Boids
	{
		boids.at(i)->run(boids, dt);
	}

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

void Actor::setAStar(AStar* aStar)
{
	this->aStar = aStar;
}

Status Actor::shoot()
{
	float tempDelta = deltaTime + this->leftoverTime;

	if (tempDelta <= this->weapon.fireSpeed)
	{
		this->leftoverTime = tempDelta;
	}

	while (tempDelta > this->weapon.fireSpeed)
	{
		tempDelta -= this->weapon.fireSpeed;
		int freeToUse = 0;

		while (freeToUse < bulletCount && this->bullets[freeToUse].timeLeft > 0.0f)
		{
			freeToUse++;
		}

		if (freeToUse < bulletCount)
		{
			Vector3 dir = (targetPos - this->getPosition());
			this->bullets[freeToUse].dir = Vector3(dir.x, 0, dir.z);
			this->bullets[freeToUse].dir.Normalize();
			this->bullets[freeToUse].timeLeft = this->weapon.bulletLifetime;
			this->bullets[freeToUse].speed = this->weapon.bulletSpeed;
			this->bullets[freeToUse].obj->setPosition(this->getPosition() + Vector3(0, 2, 0));
			this->bullets[freeToUse].obj->setRotation(Vector3(XMVector3AngleBetweenVectors(Vector3(0, 0, 1), this->bullets[freeToUse].dir)) * Vector3(0, 1, 0));
		}
		else
		{
			this->leftoverTime = 0.0f;
			break;
		}

		if (tempDelta <= this->weapon.fireSpeed)
		{
			this->leftoverTime = tempDelta;
		}
	}
	return Status::SUCCESS;
}

void Actor::chase()
{
	findPath();

	//Vector3 dir = targetPos - this->getPosition();
	//dir.Normalize();
	//Vector3 newPosition = this->getPosition() + dir * deltaTime;
	//this->setPosition(newPosition);

	//for (int i = 0; i < boids.size(); i++) //Updating Boids
	//{
	//	boids.at(i)->setDestination(targetPos);
	//}
}

void Actor::roam()
{
	//Hitta random position och hitta väg till den.


	findPath();
}

Status Actor::inRange()
{
	Status status;

	if ((getPosition() - targetPos).Length() > 7)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Actor::enemyNear()
{
	Status status;

	if ((getPosition() - targetPos).Length() > 20)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Actor::setChaseState()
{
	this->state = State::Chasing;
	return Status::SUCCESS;
}

Status Actor::setRoamState()
{
	this->state = State::Roaming;
	return Status::SUCCESS;
}

void Actor::followPath()
{
	if (path.size() > 0)
	{
		targetNode = DirectX::SimpleMath::Vector3(float(path.at(path.size() - 1)->GetXPos()),
			.0f,
			float(path.at(path.size() - 1)->GetYPos()));
		Vector3 dir = targetNode - this->getPosition();
		dir.Normalize();
		Vector3 newPosition = this->getPosition() + dir * deltaTime;

		for (int i = 0; i < boids.size(); i++) //Updating Boids
		{
			boids.at(i)->setDestination(targetNode);
		}

		if (newPosition.Distance(targetNode, newPosition) < 1)
		{
			path.pop_back();
		}
	this->setPosition(newPosition);
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
	}

}

void Actor::findPath()
{
	aStar->algorithm(this->getPosition(), targetPos, path);
}
void Actor::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Actor::inRange, std::ref(*this)));
	Behavior& follow = bt.getAction();
	follow.addAction(std::bind(&Actor::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Actor::setRoamState, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);
	sequence.addChildren(inRange);
	sequence.addChildren(follow);
}