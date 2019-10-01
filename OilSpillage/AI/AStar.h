#pragma once
#include<vector>
#include<d3d11.h>
#include<SimpleMath.h>
#include"Node.h"
class AStar
{
private:
	int getDistance(DirectX::SimpleMath::Vector2 pos1, DirectX::SimpleMath::Vector2 pos2);
	void addToVector(std::vector<Node*>& nodes, Node* nodeToAdd);
	bool isInVector(std::vector<Node*> closed, Node* node);
	Node* getNode(DirectX::SimpleMath::Vector3 position);
	std::vector<Node*> reconstructPath(Node* goal);
	std::vector<Node*> nodes;
	int gridWidth;
	int gridHeight;
public:
	std::vector<Node*> algorithm(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 endPos);
	AStar();
	~AStar();
	AStar(int gridWidth, int gridHeight);

};