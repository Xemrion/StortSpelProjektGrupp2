#include "Node.h"

void Node::reset()
{
	this->gCost = 0;
	this->hCost = 0;
	this->fCost = 0;
	this->previousNode = nullptr;
}

void Node::SetGCost(int gCost)
{
	this->gCost = gCost;
}

void Node::SetHCost(int hCost)
{
	this->hCost = hCost;
}

void Node::SetFCost(int fCost)
{
	this->fCost = fCost;
}

void Node::SetXPos(int xPos)
{
	this->Pos.x = xPos;
}

void Node::SetYPos(int yPos)
{
	this->Pos.y = yPos;
}

int Node::GetGCost()
{
	return this->gCost;
}

int Node::GetHCost()
{
	return this->hCost;
}

int Node::GetFCost()
{
	return this->fCost;
}

DirectX::SimpleMath::Vector2 Node::GetPos()
{
	return this->Pos;
}



int Node::GetXPos()
{
	return this->Pos.x;
}

int Node::GetYPos()
{
	return this->Pos.y;
}

int Node::GetID()
{
	return this->ID;
}

void Node::SetID(int ID)
{
	this->ID = ID;
}

bool Node::IsTraversable()
{
	return traversable;
}

void Node::SetTraversable(bool isTraversable)
{
	this->traversable = isTraversable;
}

void Node::AddNeighbour(Node* neighbour)
{
	this->neighbours.push_back(neighbour);
}

std::vector<Node*> Node::GetNeighbours()
{
	return this->neighbours;
}

Node* Node::GetPreviousNode()
{
	return this->previousNode;
}

void Node::SetPreviousNode(Node* node)
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
	return left->Pos.x == right.Pos.x && left->Pos.y == right.Pos.y;
	//return left->ID == right.ID;
}

bool operator==(const Node& left, Node& right)
{
	return left.Pos.x == right.Pos.x && left.Pos.y == right.Pos.y;
	//return left.ID == right.ID;
}
