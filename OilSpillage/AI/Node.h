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
	void setGCost(int gCost);
	void setHCost(int hCost);
	void setFCost(int fCost);
	void setXPos(int xPos);
	void setYPos(int yPos);
	int getGCost();
	int getHCost();
	int getFCost();
	DirectX::SimpleMath::Vector2 getPos();
	friend bool operator==(const Node* left, Node& right);
	friend bool operator==(const Node& left, Node& right);
	float getXPos();
	float getYPos();
	int getID();
	void setID(int ID);
	bool isTraversable();
	void setTraversable(bool isTraversable);
	void addNeighbour(Node* neighbour);
	std::vector<Node*> getNeighbours();
	Node* getPreviousNode();
	void setPreviousNode(Node* node);
	Node();
};