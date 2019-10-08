#include "Behaviour.h"
/*  A sequence will visit each child in order,
starting with the first, and when that succeeds will call the second,
and so on down the list of children.
If any child fails it will immediately return failure to the parent.
If the last child in the sequence succeeds,
then the sequence will return success to its parent.
*/

Status Sequence::func()
{
	//Set a default value on status
	Status status = Status::INVALID;

	for (size_t i = 0; i < m_children.size() &&
		status != Status::FAILURE &&// if fail dont go further down
		status != Status::RUNNING; i++) //if Running, we are not done with the task til its succeeded
	{
		Selector* selector = dynamic_cast<Selector*>(m_children[i]);
		if (selector)
		{
			status = selector->func(); // safe to call
		}
		else {//we got a leaf

			status = m_children[i]->execute();
		}

	}
	return status;
}



/*Where a sequence is an AND,
requiring all children to succeed to return a success,
a selector will return a success if any of its children succeed and not process any further children.
It will process the first child, and if it fails will process the second,
and if that fails will process the third, until a success is reached,
at which point it will instantly return success. It will fail if all children fail.*/

Status Selector::func()
{
	/*
	for (auto c : m_children)
	{
		if (Sequence * se = dynamic_cast<Sequence*>(&c))
		{
			se->func(); // safe to call
		}
		else
			c.execute();
	}*/
	//Set a default value on status
	Status status = Status::INVALID;

	for (size_t i = 0; i < m_children.size() &&
		status != Status::SUCCESS &&// if we succeed lets return it
		status != Status::RUNNING; i++) //if Running, we are not done with the task til its succeeded
	{
		Sequence* sequence = dynamic_cast<Sequence*>(m_children[i]);
		if (sequence)
		{
			status = sequence->func(); // safe to call
		}
		else {//we got a leaf

			status = m_children[i]->execute();
		}

	}
	return status;
}
