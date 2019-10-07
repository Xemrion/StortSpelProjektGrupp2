#pragma once
#include<vector>
#include<d3d11.h>
#include<SimpleMath.h>
#include"Node.h"
using namespace DirectX::SimpleMath;
class AStar
{
private:
	int getDistance(Vector2 pos1, Vector2 pos2);
	void addToVector(std::vector<Node*>& nodes, Node* nodeToAdd);
	bool isInVector(std::vector<Node*> vector, Node* node);
	Node* getNode(Vector3 position);
	void resetNodes();
	std::vector<Node*> reconstructPath(Node* goal);
	std::vector<Node*> nodes;
	int gridWidth;
	int gridHeight;
public:
	std::vector<Node*> algorithm(Vector3 startPos, Vector3 endPos);
	AStar();
	~AStar();
	AStar(int gridWidth, int gridHeight, Vector2 topLeftCoord);

};