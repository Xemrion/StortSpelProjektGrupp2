#include"ObjectiveHandler.h"
#include"..////game.h"
#include"..//States/PlayingGameState.h"
ObjectiveHandler::ObjectiveHandler()
{
	this->types.types[0] = " Boxes ";
	this->types.types[1] = " Reactors ";
	this->types.types[2] = " Crates ";
	this->types.types[3] = " Scraps ";

	this->types.types[0] = " Boxes ";
	this->types.types[1] = " Reactors ";
	this->types.types[2] = " Crates ";
	this->types.types[3] = " Scraps ";

	this->types.colors[0] = Vector4(0.7f, 1.0f, 0.3f, 1.0f);
	this->types.colors[1] = Vector4(0.3f, 0.7f, 1.0f, 1.0f);
	this->types.colors[2] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	this->types.colors[3] = Vector4(1.0f, 0.7f, 1.0f, 1.0f);
	Game::getGraphics().loadTexture("crate");
	Game::getGraphics().loadTexture("crateSpec");

}

ObjectiveHandler::~ObjectiveHandler()
{
	
	for (int i = 0; i < this->pickUpArrs.size(); i++)
	{
		int max = 0;
		for (int j = 0; j < this->nrOfPickupsVec.at(i); j++)
		{
			delete this->pickUpArrs.at(i)[j];
		}
		delete this->pickUpArrs.at(i);
	}
	for (int i = 0; i < this->objectiveVec.size(); i++)
	{
		delete this->objectiveVec.at(i);
	}
	this->objectiveVec.clear();

}

void ObjectiveHandler::addObjective(TypeOfMission type, int rewardTime,int nrOfTargets, std::string info, TypeOfTarget targetType,Vector3 getToPos, GameObject** target, Boss* boss)
{
	PlayingGameState* ptrState = static_cast<PlayingGameState*>(Game::getCurrentState());
	Objective *temp = new Objective;
	temp->setType(type);
	int typeInt = rand() % int(TypeOfTarget::Size);
	if (targetType != TypeOfTarget::Size)
	{
		typeInt = int(targetType);
	}
	std::string typeInfo = this->types.getType(TypeOfTarget(typeInt));
	
	temp->setReward(rewardTime);
	temp->setScore(500 * (1 + (0.1*Game::getGameInfo().nrOfClearedStages)));
	if (type == TypeOfMission::FindAndCollect)
	{
		temp->setInfo(info + typeInfo);
		this->nrOfPickupsVec.push_back(nrOfTargets);
		GameObject** targets;
		targets = new GameObject * [nrOfTargets];
		temp->setTargetType(TypeOfTarget(typeInt));
		for (int i = 0; i < nrOfTargets; i++)
		{
			targets[i] = new GameObject;
			Vector3 pos(rand() % 10 + 10, 0, rand() % 10 - 20);
			pos.x += i * 10;
			pos.z += i * -10;
			targets[i]->mesh = Game::getGraphics().getMeshPointer("Cube");
			targets[i]->setRotation(Vector3(0, (23 + 0.3f * 3.14 * (rand() % 200))-(23+0.3f*3.14*(rand()%400)),0));
			targets[i]->setScale(Vector3((rand()%3) * 0.1f + 1.0f));
			pos.y = -1.5f + targets[i]->getScale().y * 0.5f;
			targets[i]->setPosition(pos);

			
			if (TypeOfTarget(typeInt) == TypeOfTarget::Crate)
			{
				targets[i]->setTexture(Game::getGraphics().getTexturePointer("crate"));
				targets[i]->setSpecularMap(Game::getGraphics().getTexturePointer("crateSpec"));

			}
			else
			{
				targets[i]->setColor(this->types.getColor(TypeOfTarget(typeInt)));
			}
			
		}
		this->pickUpArrs.push_back(targets);
		temp->setTarget(this->pickUpArrs.at(this->pickUpArrs.size()-1), nrOfTargets);
	}
	else if (type == TypeOfMission::GetToPoint)
	{
		temp->setInfo(info);
		temp->setGeneralPosition(getToPos);
		SpotLight newSpot = SpotLight(getToPos+Vector3(0.0f,4.0f,0.0f), Vector3(1, 0, 0), 0, Vector3(0, -1, 0), 45);
		temp->setSpotLight(static_cast<PlayingGameState*>(Game::getCurrentState())->addLight(newSpot));
	}
	else
	{
		temp->setInfo(info);
		temp->setEnemies(nrOfTargets);
		temp->setBoss(boss);
	}
	this->objectiveVec.push_back(temp);
}

bool ObjectiveHandler::isAllDone()
{
	bool allDone = false;
	for (Objective* object : this->objectiveVec)
	{
		allDone = object->isDone();
		if (allDone == false)
		{
			break;//om den hittar minst en så är alla inte klara
		}
	}
	if (this->objectiveVec.size() == 0)
	{
		return true;
	}
	return allDone;
}

void ObjectiveHandler::update(Vector3 playerPos, Physics* physics)
{
	//check which pickUpArrs item is closest to player
	if (this->objectiveVec.size() != 0)
	{
		int shortestDistObj = 0;
		float distanceFromPlayerToPickup = 0.0f;
		float distanceFromPlayerToPickupShortest = 0.0f;

		for (int i = 0; i < this->objectiveVec.at(0)->getNrOfMax(); i++) // 5
		{
			Vector3 pos = this->pickUpArrs.at(0)[i]->getPosition();
			Vector3 posShortest = this->pickUpArrs.at(0)[shortestDistObj]->getPosition();
			int stop = 1;

			distanceFromPlayerToPickup =
					sqrtf((pos.x - playerPos.x) * (pos.x - playerPos.x) +
						  (pos.y - playerPos.y) * (pos.y - playerPos.y) +
						  (pos.z - playerPos.z) * (pos.z - playerPos.z));

			distanceFromPlayerToPickupShortest =
					sqrtf((posShortest.x - playerPos.x) * (posShortest.x - playerPos.x) +
						  (posShortest.y - playerPos.y) * (posShortest.y - playerPos.y) +
						  (posShortest.z - playerPos.z) * (posShortest.z - playerPos.z));

			if (distanceFromPlayerToPickup <= distanceFromPlayerToPickupShortest)
			{
				shortestDistObj = i;
				distanceFromPlayerToPickupShortest = distanceFromPlayerToPickup;
			}
		}

		//calculate vector towards player and speed
		Vector3 towardsPlayer = playerPos - this->pickUpArrs.at(0)[shortestDistObj]->getPosition();
		towardsPlayer.Normalize(); //so not faster if closer
		float velocityIncrease = 0;

		if (distanceFromPlayerToPickupShortest <= 17.5) //do another check for player hp
		{
			velocityIncrease = 10; //speed if close
		}
		else
		{
			velocityIncrease = 0; //no speed not close
		}

		//remove last rigidbody so we dont get too many
		if (this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody() != nullptr)
			physics->DeleteRigidBody(this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody());

		btRigidBody* tempo = physics->addSphere(1.0f, btVector3
				(this->pickUpArrs.at(0)[shortestDistObj]->getPosition().x, 
				 this->pickUpArrs.at(0)[shortestDistObj]->getPosition().y, 
				 this->pickUpArrs.at(0)[shortestDistObj]->getPosition().z),
				 1.5f, this->pickUpArrs.at(0)[shortestDistObj]);

		this->pickUpArrs.at(0)[shortestDistObj]->setRigidBody(tempo, physics);
		this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody()->activate();
		this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
		this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody()->setFriction(0);
		this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody()->setLinearFactor(btVector3(1, 0, 1));

		//set speed
		this->pickUpArrs.at(0)[shortestDistObj]->getRigidBody()->setLinearVelocity(btVector3(towardsPlayer.x * velocityIncrease, 0.0f, towardsPlayer.z * velocityIncrease));
	}



	if (this->objectiveVec.size() != 0)
	{
		this->objectiveVec.at(0)->update(playerPos);

		if (this->objectiveVec.at(0)->isDone())
		{
			if (this->objectiveVec.at(0)->getType()==TypeOfMission::FindAndCollect)
			{
				for (int j = 0; j < this->objectiveVec.at(0)->getNrOfMax(); j++)
				{
					Game::getGraphics().removeFromDraw(this->pickUpArrs.at(0)[j]);
				}
			}
			static_cast<PlayingGameState*>(Game::getCurrentState())->changeTime(this->objectiveVec.at(0)->getRewardTime());
			Game::getGameInfo().addHighScore(objectiveVec[0]->getScore());
			Game::getGameInfo().nrOfObjectives++;
			delete this->objectiveVec.at(0);
			this->objectiveVec.erase(this->objectiveVec.begin());
			this->eventNewObj = true;
		}
	}
}

Type ObjectiveHandler::getTypes() const
{
	return this->types;
}

void ObjectiveHandler::reset()
{
	this->objectiveVec.clear();
}

Objective* ObjectiveHandler::getObjective(int id) const
{
	if (this->objectiveVec.size() != 0)
	{
		return this->objectiveVec.at(id);
	}
	return nullptr;
}

bool& ObjectiveHandler::getEventNewObj()
{
	return this->eventNewObj;
}
