#include "AStar.h"


bool AStar::algorithm(Vector3 startPos,Vector3 endPos, std::vector<Node*>& path)
{
	path.clear();
	resetNodes();
	Node* start = getNode(startPos);
	Node* goal = getNode(endPos);
	std::vector<Node*> open;
	std::vector<Node*> closed;
	Node* current;
	open.push_back(start);
	Node* neighbour;
	if (start == nullptr || goal == nullptr)
	{
		return false;
	}
	while (!open.empty())
	{
		current = open.at(open.size() - 1);
		if (current == goal)
		{
			reconstructPath(goal,path);
			return true;
		}
		open.pop_back();
		closed.push_back(current);
		for (int i = 0; i < current->GetNeighbours().size(); i++)
		{
			neighbour = current->GetNeighbours().at(i);
			//If it is traversable and isn't in closed
			if (neighbour->IsTraversable() && !isInVector(closed, neighbour))
			{
				//Calculate new path
				int newGCost = current->GetGCost() + getDistance(neighbour->GetPos(), current->GetPos());
				int newHCost = getDistance(neighbour->GetPos(), goal->GetPos());
				int newFCost = newHCost + newGCost;

				if (!isInVector(open, neighbour) || newFCost < neighbour->GetFCost())
				{
					//G and H For debugging
					neighbour->SetGCost(newGCost);
					neighbour->SetHCost(newHCost);
					neighbour->SetFCost(newFCost);
					neighbour->SetPreviousNode(current);
					if (!isInVector(open, neighbour))
						addToVector(open, neighbour);

				}
			}
		}

	}
	std::vector<Node*> failed;
	return false;
}

Node* AStar::getNode(Vector3 position)
{
	for (int i = 0; i < gridHeight * gridWidth; i++)
	{
		if (nodes.at(i)->GetXPos() + nodes.at(i)->nodeWidth >= position.x &&
			nodes.at(i)->GetXPos() - nodes.at(i)->nodeWidth <= position.x &&
			nodes.at(i)->GetYPos() + nodes.at(i)->nodeWidth >= position.z &&
			nodes.at(i)->GetYPos() - nodes.at(i)->nodeWidth <= position.z)
		{
			return nodes.at(i);
		}
	}
	return nullptr;
}

AStar::AStar()
{
}
AStar::~AStar()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		delete nodes.at(i);
	}
	nodes.clear();

}
AStar::AStar(int gridWidth, int gridHeight,Vector2 topLeftCoord)
{
	//Create grid
	this->gridWidth = gridWidth;
	this->gridHeight = gridHeight;
	for (int i = 0; i < gridWidth * gridHeight; i++)
	{
		Node* temp = new Node();
		temp->SetID(i);
		temp->SetXPos(topLeftCoord.x + i % gridWidth);
		temp->SetYPos(topLeftCoord.y - i / gridWidth);
		nodes.push_back(temp);
	}
	for (int i = 0; i < gridWidth * gridHeight; i++)
	{
		if (i % gridWidth != 0)
		{
			if (i + gridWidth < gridWidth * gridHeight)
				nodes.at(i)->AddNeighbour(nodes.at(i + gridWidth - 1));//Lower left

			nodes.at(i)->AddNeighbour(nodes.at(i - 1));//Left
			if (i - gridWidth > 0)
				nodes.at(i)->AddNeighbour(nodes.at(i - gridWidth - 1)); //Upper left
		}
		if (i - gridWidth >= 0)
		{
			nodes.at(i)->AddNeighbour(nodes.at(i - gridWidth)); //Upper mid
			if (i % gridWidth != gridWidth - 1)
				nodes.at(i)->AddNeighbour(nodes.at(i - gridWidth + 1)); //upper right
		}
		if (i % gridWidth != gridWidth - 1)
		{
			nodes.at(i)->AddNeighbour(nodes.at(i + 1)); //Right
			if (i + gridWidth < gridWidth * gridHeight)
			{
				nodes.at(i)->AddNeighbour(nodes.at(i + gridWidth + 1)); //Lower right
			}
		}
		if (i + gridWidth < gridWidth * gridHeight)
		{
			nodes.at(i)->AddNeighbour(nodes.at(i + gridWidth)); //Lower mid
		}

	}
}
int AStar::getDistance(Vector2 pos1, Vector2 pos2)
{
	int x1 = int( pos1.x - pos2.x );
	int y1 = int( pos1.y - pos2.y );
	return int( sqrt(pow(x1, 2) + pow(y1, 2)) * 10 );
}

void AStar::addToVector(std::vector<Node*>& nodes, Node* nodeToAdd)
{
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodeToAdd->GetFCost() >= nodes.at(i)->GetFCost())
		{
			nodes.insert(nodes.begin() + i, nodeToAdd);
			return;
		}
	}
	nodes.push_back(nodeToAdd);
}

bool AStar::isInVector(std::vector<Node*> vector, Node* node)
{
	for (int i = 0; i < vector.size(); i++)
	{
		if (vector.at(i) == node)
			return true;
	}
	return false;
}

void AStar::reconstructPath(Node* goal, std::vector<Node*>& path)
{
	Node* current;
	current = goal;
	while (current->GetPreviousNode() != nullptr)
	{
		path.push_back(current);
		current = current->GetPreviousNode();
	}
}

void AStar::resetNodes()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes.at(i)->reset();
	}
}
