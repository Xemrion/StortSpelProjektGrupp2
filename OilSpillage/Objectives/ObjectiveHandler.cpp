#include"ObjectiveHandler.h"
#include"..////game.h"
#include"..//States/PlayingGameState.h"
ObjectiveHandler::ObjectiveHandler()
{
	this->types.types[0] = " Boxes ";
	this->types.types[1] = " Reactors ";
	this->types.types[2] = " Crates ";
	this->types.types[3] = " Scraps ";

	this->types.colors[0] = Vector4(0.7f, 1.0f, 0.3f, 1.0f);
	this->types.colors[1] = Vector4(0.3f, 0.7f, 1.0f, 1.0f);
	this->types.colors[2] = Vector4(0.4f, 0.2f, 0.5f, 1.0f);
	this->types.colors[3] = Vector4(1.0f, 0.7f, 1.0f, 1.0f);
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

void ObjectiveHandler::addObjective(TypeOfMission type, int rewardTime,int nrOfTargets, std::string info, GameObject** target, Actor** enemies)
{
	PlayingGameState* ptrState = static_cast<PlayingGameState*>(Game::getCurrentState());
	Objective *temp = new Objective;
	temp->setType(type);
	int typeInt = rand() % int(TypeOfTarget::Size);
	std::string typeInfo = this->types.getType(TypeOfTarget(typeInt));
	
	temp->setReward(rewardTime);
	
	if (type == TypeOfMission::FindAndCollect)
	{
		temp->setInfo(info + typeInfo);
		this->nrOfPickupsVec.push_back(nrOfTargets);
		GameObject** targets;
		targets = new GameObject * [nrOfTargets];
		for (int i = 0; i < nrOfTargets; i++)
		{
			targets[i] = new GameObject;
			Vector3 pos(rand() % 10 + 10, 0, rand() % 10 - 20);
			pos.x += i * 10;
			pos.z += i * -10;
			pos.y -= 1 - this->types.getColor(TypeOfTarget(typeInt)).z;
			pos += ptrState->generateObjectivePos(20, 50);

			targets[i]->setPosition(pos);
			targets[i]->mesh = Game::getGraphics().getMeshPointer("Cube");
			targets[i]->setColor(this->types.getColor(TypeOfTarget(typeInt)));
			targets[i]->setScale(Vector3(this->types.getColor(TypeOfTarget(typeInt)).z));
			Game::getGraphics().addToDraw(targets[i]);
		}
		this->pickUpArrs.push_back(targets);
		temp->setTarget(this->pickUpArrs.at(this->pickUpArrs.size()-1), nrOfTargets);
	}
	else
	{
		temp->setInfo(info);
		temp->setEnemies(enemies, nrOfTargets);
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
	return allDone;
}

void ObjectiveHandler::update(Vector3 playerPos)
{
	if (this->objectiveVec.size() != 0)
	{
		this->objectiveVec.at(0)->update(playerPos);
		if (this->objectiveVec.at(0)->isDone())
		{
			if (this->objectiveVec.at(0)->getType()==TypeOfMission::FindAndCollect)
			{
			
				for (int i = 0; i < 1; i++)
				{
					for (int j = 0; j < this->objectiveVec.at(i)->getNrOfMax(); j++)
					{
						Game::getGraphics().removeFromDraw(this->pickUpArrs.at(i)[j]);
					}
				}
				
			
			}
			static_cast<PlayingGameState*>(Game::getCurrentState())->addTime(this->objectiveVec.at(0)->getRewardTime());
			delete this->objectiveVec.at(0);
			this->objectiveVec.erase(this->objectiveVec.begin());
			this->eventNewObj = true;
		}
	}
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
