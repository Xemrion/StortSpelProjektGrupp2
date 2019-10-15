#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <functional>
#include <iostream>
#include <vector>
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
	Behavior();
	Behavior(ActionCallback func);

	virtual void addAction(ActionCallback func);

	Status execute();
};
class Sequence :public Behavior
{
private:
	std::vector<Behavior*> children;
public:
	virtual	void addChildren(Behavior& child);
	Status func();
	~Sequence();
};

class Selector :public Behavior
{
private:
	std::vector<Behavior*> children;
public:
	void addChildren(Behavior& child);
	Status func();
	~Selector();
};


class BT
{
private:
	std::vector<Sequence*> sequence;
	std::vector<Selector*> selector;
	std::vector<Behavior*> action;
public:
	Sequence& getSequence();
	Selector& getSelector();
	Behavior& getAction();
	~BT();
};
#endif // ! BEHAVIOUR_H
