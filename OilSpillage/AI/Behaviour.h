#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <functional>
#include <iostream>
#include <vector>
using namespace std::placeholders;

class Selector;
class Sequence;
enum class Status
{
	INVALID,
	RUNNING,
	SUCCESS,
	FAILURE,
	WAIT
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
class Randomize : public Behavior
{
private:
	std::vector<Behavior*> m_children;
	/*status on the random action */
	Status randomize;
	uint8_t index;
public:
	void addChildren(Behavior& child);
	Status func();
	~Randomize();

};

struct WaitTimer
{
	float wait_time = 0;
	float time_left = 0;
	bool repeatable = false;
};
class BT
{
private:
	std::vector<Sequence*> sequence;
	std::vector<Selector*> selector;
	std::vector<Behavior*> action;
	std::vector<Randomize*> random;
public:
	Sequence& getSequence();
	Selector& getSelector();
	Behavior& getAction();
	Randomize& getRandom();
	~BT();
};
#endif // ! BEHAVIOUR_H
