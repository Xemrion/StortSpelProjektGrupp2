#include "Node.h"

void Node::SetGCost(int gCost)
{
	this->g_cost = gCost;
}

void Node::SetHCost(int hCost)
{
	this->h_cost = hCost;
}

void Node::SetFCost(int fCost)
{
	this->f_cost = fCost;
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
	return this->g_cost;
}

int Node::GetHCost()
{
	return this->h_cost;
}

int Node::GetFCost()
{
	return this->f_cost;
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
	this->g_cost = 0;
	this->h_cost = 0;
	this->f_cost = 0;
	this->Pos = DirectX::SimpleMath::Vector2(0, 0);
	this->previousNode = nullptr;
	this->traversable = true;
}

bool operator==(const Node* left, Node& right)
{
	return left->Pos.x == right.Pos.x && left->Pos.y == right.Pos.y;
}

bool operator==(const Node& left, Node& right)
{
	return left.Pos.x == right.Pos.x && left.Pos.y == right.Pos.y;
}
