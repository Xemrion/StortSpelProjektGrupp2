#include "Node.h"

void Node::reset()
{
	this->gCost = 0;
	this->hCost = 0;
	this->fCost = 0;
	this->previousNode = nullptr;
}

void Node::setGCost(int gCost)
{
	this->gCost = gCost;
}

void Node::setHCost(int hCost)
{
	this->hCost = hCost;
}

void Node::setFCost(int fCost)
{
	this->fCost = fCost;
}

void Node::setXPos(int xPos)
{
	this->Pos.x = xPos;
}

void Node::setYPos(int yPos)
{
	this->Pos.y = yPos;
}

int Node::getGCost()
{
	return this->gCost;
}

int Node::getHCost()
{
	return this->hCost;
}

int Node::getFCost()
{
	return this->fCost;
}

DirectX::SimpleMath::Vector2 Node::getPos()
{
	return this->Pos;
}

float Node::getXPos()
{
	return float(this->Pos.x);
}

float Node::getYPos()
{
	return float(this->Pos.y);
}

int Node::getID()
{
	return this->ID;
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
	this->traversable = isTraversable;
}

void Node::addNeighbour(Node* neighbour)
{
	this->neighbours.push_back(neighbour);
}

std::vector<Node*> Node::getNeighbours()
{
	return this->neighbours;
}

Node* Node::getPreviousNode()
{
	return this->previousNode;
}

void Node::setPreviousNode(Node* node)
{
	this->previousNode = node;
}

Node::Node()
{
	this->ID = 0;
	this->gCost = 0;
	this->hCost = 0;
	this->fCost = 0;
	this->Pos = DirectX::SimpleMath::Vector2(0, 0);
	this->previousNode = nullptr;
	this->traversable = true;
}

bool operator==(const Node* left, Node& right)
{
	//return left->Pos.x == right.Pos.x && left->Pos.y == right.Pos.y;
	return left->ID == right.ID;
}

bool operator==(const Node& left, Node& right)
{
	//return left.Pos.x == right.Pos.x && left.Pos.y == right.Pos.y;
	return left.ID == right.ID;

}
