#include "Node.h"

void Node::SetGCost(int gCost)
{
	g_cost = gCost;
}

void Node::SetHCost(int hCost)
{
	h_cost = hCost;
}

void Node::SetFCost(int fCost)
{
	f_cost = fCost;
}

void Node::SetXPos(int xPos)
{
	Pos.x = float( xPos );
}

void Node::SetYPos(int yPos)
{
	Pos.y = float( yPos );
}

int Node::GetGCost()
{
	return g_cost;
}

int Node::GetHCost()
{
	return h_cost;
}

int Node::GetFCost()
{
	return f_cost;
}

DirectX::SimpleMath::Vector2 Node::GetPos()
{
	return Pos;
}



int Node::GetXPos()
{
	return int( Pos.x );
}

int Node::GetYPos()
{
	return int( Pos.y );
}

int Node::GetID()
{
	return ID;
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
	traversable = isTraversable;
}

void Node::AddNeighbour(Node* neighbour)
{
	neighbours.push_back(neighbour);
}

std::vector<Node*> Node::GetNeighbours()
{
	return neighbours;
}

Node* Node::GetPreviousNode()
{
	return previousNode;
}

void Node::SetPreviousNode(Node* node)
{
	previousNode = node;
}

Node::Node()
{
	ID           = 0;
	g_cost       = 0;
	h_cost       = 0;
	f_cost       = 0;
	Pos          = DirectX::SimpleMath::Vector2(0, 0);
	previousNode = nullptr;
	traversable  = true;
}

bool operator==(const Node* left, Node& right)
{
	return left->Pos.x == right.Pos.x && left->Pos.y == right.Pos.y;
}

bool operator==(const Node& left, Node& right)
{
	return left.Pos.x == right.Pos.x && left.Pos.y == right.Pos.y;
}
