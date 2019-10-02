#pragma once
#include<d3d11.h>
#include<SimpleMath.h>
#include<vector>
class Node {

private:
	int gCost;
	int hCost;
	int fCost;
	DirectX::SimpleMath::Vector2 Pos;
	bool traversable;
	std::vector<Node*> neighbours;
	Node* previousNode;
public:
	void reset();
	int ID;
	float nodeWidth = 0.5f;
	void SetGCost(int gCost);
	void SetHCost(int hCost);
	void SetFCost(int fCost);
	void SetXPos(int xPos);
	void SetYPos(int yPos);
	int GetGCost();
	int GetHCost();
	int GetFCost();
	DirectX::SimpleMath::Vector2 GetPos();
	friend bool operator==(const Node* left, Node& right);
	friend bool operator==(const Node& left, Node& right);
	int GetXPos();
	int GetYPos();
	int GetID();
	void SetID(int ID);
	bool IsTraversable();
	void SetTraversable(bool isTraversable);
	void AddNeighbour(Node* neighbour);
	std::vector<Node*> GetNeighbours();
	Node* GetPreviousNode();
	void SetPreviousNode(Node* node);
	Node();
};