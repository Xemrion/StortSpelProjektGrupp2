#include "Attacker.h"

Attacker::Attacker()
{
	setUpActor();
}

Attacker::~Attacker()
{
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inRange, std::ref(*this)));
	Behavior& follow = bt.getAction();
	follow.addAction(std::bind(&Attacker::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Attacker::setRoamState, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);
	sequence.addChildren(inRange);
	sequence.addChildren(follow);
}
