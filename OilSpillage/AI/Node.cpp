#include "Node.h"

void Node::setGCost(int gCost)
{
	gCost = gCost;
}

void Node::setHCost(int hCost)
{
	hCost = hCost;
}

void Node::setFCost(int fCost)
{
	fCost = fCost;
}

void Node::setXPos(int xPos)
{
	pos.x = float(xPos);
}

void Node::setYPos(int yPos)
{
	pos.y = float(yPos);
}

int Node::getGCost()
{
	return gCost;
}

int Node::getHCost()
{
	return hCost;
}

int Node::getFCost()
{
	return fCost;
}

DirectX::SimpleMath::Vector2 Node::getPos()
{
	return pos;
}



void Node::reset()
{

	this->gCost = 0;
	this->hCost = 0;
	this->fCost = 0;
	this->setPreviousNode(nullptr);
}

int Node::getXPos()
{
	return int(pos.x);
}

int Node::getYPos()
{
	return int(pos.y);
}

int Node::getID()
{
	return ID;
}

void Node::setID(int ID)
{
	this->ID = ID;
}

bool Node::isTraversable()
{
	return traversable;
}

void Node::setTraversable(bool isTraversable)
{
	traversable = isTraversable;
}

void Node::addNeighbour(Node* neighbour)
{
	neighbours.push_back(neighbour);
}

std::vector<Node*> Node::getNeighbours()
{
	return neighbours;
}

Node* Node::getPreviousNode()
{
	return previousNode;
}

void Node::setPreviousNode(Node* node)
{
	previousNode = node;
}

Node::Node()
{
	ID = 0;
	gCost = 0;
	hCost = 0;
	fCost = 0;
	pos = DirectX::SimpleMath::Vector2(0, 0);
	previousNode = nullptr;
	traversable = true;
}

bool operator==(const Node* left, Node& right)
{
	return left->pos.x == right.pos.x && left->pos.y == right.pos.y;
}

bool operator==(const Node& left, Node& right)
{
	return left.pos.x == right.pos.x && left.pos.y == right.pos.y;
}
