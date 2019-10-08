#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <functional>
#include <iostream>
#include <vector>
using namespace std;
using namespace std::placeholders;

class Selector;
class Sequence;
enum Status
{
	INVALID,
	RUNNING,
	SUCCESS,
	FAILURE
};
using ActionCallback = std::function<Status()>;
class Behavior
{
private:
	ActionCallback callback;
public:
	Behavior() {}
	Behavior(ActionCallback func) :callback(func) {}

	virtual void addAction(ActionCallback func)
	{
		this->callback = func;
	};

	Status execute()
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
};

//class Composite :public  Behavior
//{
//protected:
//	vector<Behavior*> m_children;
//public:
//	void AddChildren(Behavior& child)
//	{
//		m_children.emplace_back(&child);
//	}
//	/*void AddChildren(Behavior child)
//	{
//		m_children.push_back(child);
//	}*/
//	//void RemoveChild(Behavior child)
//	//{
//	//	for (size_t i = 0; i < m_children.size(); i++)
//	//	{
//	//		if (c == child)
//	//		{
//	//			m_children.erase(m_children.begin() + i);
//	//		}
//	//	}
//	//}
//};
class Sequence :public Behavior
{
private:
	vector<Behavior*> m_children;
public:
	virtual	void AddChildren(Behavior& child)
	{
		m_children.emplace_back(&child);
	}
	Status func();
	~Sequence()
	{

	}
};

class Selector :public Behavior
{
private:
	vector<Behavior*> m_children;
public:
	void AddChildren(Behavior& child)
	{
		m_children.emplace_back(&child);
	}
	Status func();
	~Selector()
	{

	}
};


class BT
{
private:
	vector<Sequence*> sequence;
	vector<Selector*> selector;
	vector<Behavior*> action;
public:
	Sequence& GetSequence() {
		Sequence* temp = new Sequence();
		sequence.emplace_back(temp);
		return *temp;
	}
	Selector& GetSelector() {
		Selector* temp = new Selector();
		selector.emplace_back(temp);
		return *temp;
	}
	Behavior& GetAction()
	{
		Behavior* temp = new Behavior();
		action.emplace_back(temp);
		return *temp;
	}
	~BT()
	{
		for (vector<Sequence*>::iterator pObj = sequence.begin();
			pObj != sequence.end(); ++pObj) {
			delete* pObj; // Note that this is deleting what pObj points to,
						  // which is a pointer
		}

		for (vector<Selector*>::iterator pObj = selector.begin();
			pObj != selector.end(); ++pObj) {
			delete* pObj; // Note that this is deleting what pObj points to,
						  // which is a pointer
		}

		for (vector<Behavior*>::iterator pObj = action.begin();
			pObj != action.end(); ++pObj) {
			delete* pObj; // Note that this is deleting what pObj points to,
						  // which is a pointer
		}
	}
};
#endif // ! BEHAVIOUR_H
