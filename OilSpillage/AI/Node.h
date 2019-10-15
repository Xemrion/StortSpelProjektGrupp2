#pragma once
#include<d3d11.h>
#include<SimpleMath.h>
#include<vector>
class Node {

private:
	int ID;
	int gCost; //
	int hCost;
	int fCost; //
	DirectX::SimpleMath::Vector2 pos;
	bool traversable; //
	std::vector<Node*> neighbours;
	Node* previousNode; // 
public:
	float nodeWidth = 0.5f;
	void setGCost(int gCost); //
	void setHCost(int hCost);
	void setFCost(int fCost); // 
	void setXPos(int xPos);
	void setYPos(int yPos);
	int getGCost(); //
	int getHCost();
	int getFCost(); //
	DirectX::SimpleMath::Vector2 getPos();
	friend bool operator==(const Node* left, Node& right); // 
	friend bool operator==(const Node& left, Node& right); //
	void reset(); //
	int getXPos(); 
	int getYPos();
	int getID();
	void setID(int ID);
	bool isTraversable(); //
	void setTraversable(bool isTraversable); //
	void addNeighbour(Node* neighbour);
	std::vector<Node*> getNeighbours();
	Node* getPreviousNode(); //
	void setPreviousNode(Node* node); //
	Node();
};