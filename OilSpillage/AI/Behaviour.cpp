#include "Behaviour.h"
/*  A sequence will visit each child in order,
starting with the first, and when that succeeds will call the second,
and so on down the list of children.
If any child fails it will immediately return failure to the parent.
If the last child in the sequence succeeds,
then the sequence will return success to its parent.
*/

void Sequence::addChildren(Behavior& child)
{
	children.emplace_back(&child);
}

Status Sequence::func()
{
	Status status = Status::INVALID;

	for (size_t i = 0; i < children.size() &&
		status != Status::FAILURE &&// if fail dont go further down
		status != Status::RUNNING &&
		status != Status::WAIT; i++) //if Running, we are not done with the task til its succeeded
	{
		Selector* selector = dynamic_cast<Selector*>(children[i]);
		if (selector)
		{
			status = selector->func(); // safe to call
		}
		else
		{//we got a leaf

			status = children[i]->execute();
		}

	}
	return status;
}

Sequence::~Sequence()
{
}



/*Where a sequence is an AND,
requiring all children to succeed to return a success,
a selector will return a success if any of its children succeed and not process any further children.
It will process the first child, and if it fails will process the second,
and if that fails will process the third, until a success is reached,
at which point it will instantly return success. It will fail if all children fail.*/

void Selector::addChildren(Behavior& child)
{
	children.emplace_back(&child);

}

Status Selector::func()
{
	Status status = Status::INVALID;

	for (size_t i = 0; i < children.size() &&
		status != Status::SUCCESS &&// if we succeed lets return it
		status != Status::RUNNING &&
		status != Status::WAIT	; i++) //if Running, we are not done with the task til its succeeded
	{
		Sequence* sequence = dynamic_cast<Sequence*>(children[i]);
		if (sequence)
		{
			status = sequence->func(); // safe to call
		}
		else
		{//we got a leaf

			status = children[i]->execute();
		}

	}
	return status;
}

Selector::~Selector()
{
}

Sequence& BT::getSequence()
{
	Sequence* temp = new Sequence();
	sequence.emplace_back(temp);
	return *temp;
}

Selector& BT::getSelector()
{
	Selector* temp = new Selector();
	selector.emplace_back(temp);
	return *temp;
}

Behavior& BT::getAction()
{
	Behavior* temp = new Behavior();
	action.emplace_back(temp);
	return *temp;
}

Randomize& BT::getRandom()
{
	Randomize* temp = new Randomize();
	random.emplace_back(temp);
	return *temp;
}

BT::~BT()
{
	for (std::vector<Sequence*>::iterator pObj = sequence.begin();
		pObj != sequence.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}

	for (std::vector<Selector*>::iterator pObj = selector.begin();
		pObj != selector.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}

	for (std::vector<Behavior*>::iterator pObj = action.begin();
		pObj != action.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}
	for (std::vector<Randomize*>::iterator pObj = random.begin();
		pObj != random.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}
}

Behavior::Behavior()
{
}

Behavior::Behavior(ActionCallback func) :callback(func)
{
}

void Behavior::addAction(ActionCallback func)
{
	this->callback = func;
}

Status Behavior::execute()
{
	Status status;
	//check if its empty(no function stored)
	if (callback)
	{
		status = callback();
	}

	else
	{
		status = Status::FAILURE;
	}
	return status;

}

void Randomize::addChildren(Behavior& child)
{
	m_children.emplace_back(&child);
	randomize = Status::INVALID;
	index = rand() % m_children.size();
}

Status Randomize::func()
{
	/*	choses one out of the children to execute	*/

	/*if we haven't succeed the action*/
	if (randomize != Status::SUCCESS)
	{
		Sequence* sequence = dynamic_cast<Sequence*>(m_children[index]);
		Selector* selector = dynamic_cast<Selector*>(m_children[index]);
		if (sequence)
		{
			randomize = sequence->func();
		}
		else if (selector)
		{
			randomize = selector->func();
		}
		else {

			randomize = m_children[index]->execute();
		}
	}
	if (randomize == Status::SUCCESS)
	{
		/*pick another random action to execute*/
		index = rand() % m_children.size();
		randomize = Status::RUNNING;// its still running 
	}

	return randomize;
}

Randomize::~Randomize()
{
}
