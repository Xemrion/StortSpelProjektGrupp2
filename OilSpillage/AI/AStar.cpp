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
		for (int i = 0; i < current->getNeighbours().size(); i++)
		{
			neighbour = current->getNeighbours().at(i);
			//If it is traversable and isn't in closed
			if (neighbour->isTraversable() && !isInVector(closed, neighbour))
			{
				//Calculate new path
				int newGCost = current->getGCost() + getDistance(neighbour->getPos(), current->getPos());
				int newHCost = getDistance(neighbour->getPos(), goal->getPos());
				int newFCost = newHCost + newGCost;

				if (!isInVector(open, neighbour) || newFCost < neighbour->getFCost())
				{
					//G and H For debugging
					neighbour->setGCost(newGCost);
					neighbour->setHCost(newHCost);
					neighbour->setFCost(newFCost);
					neighbour->setPreviousNode(current);
					if (!isInVector(open, neighbour))
					{
						addToVector(open, neighbour);
					}

				}
			}
		}

	}
	return false;
}

Node* AStar::getNode(Vector3 position)
{
	for (int i = 0; i < gridHeight * gridWidth; i++)
	{
		if (nodes.at(i)->getXPos() + nodes.at(i)->nodeWidth >= position.x &&
			nodes.at(i)->getXPos() - nodes.at(i)->nodeWidth <= position.x &&
			nodes.at(i)->getYPos() + nodes.at(i)->nodeWidth >= position.z &&
			nodes.at(i)->getYPos() - nodes.at(i)->nodeWidth <= position.z)
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
		temp->setID(i);
		temp->setXPos(topLeftCoord.x + i % gridWidth);
		temp->setYPos(topLeftCoord.y - i / gridWidth);
		nodes.push_back(temp);
	}
	for (int i = 0; i < gridWidth * gridHeight; i++)
	{
		if (i % gridWidth != 0)
		{
			if (i + gridWidth < gridWidth * gridHeight)
				nodes.at(i)->addNeighbour(nodes.at(i + gridWidth - 1));//Lower left

			nodes.at(i)->addNeighbour(nodes.at(i - 1));//Left
			if (i - gridWidth > 0)
				nodes.at(i)->addNeighbour(nodes.at(i - gridWidth - 1)); //Upper left
		}
		if (i - gridWidth >= 0)
		{
			nodes.at(i)->addNeighbour(nodes.at(i - gridWidth)); //Upper mid
			if (i % gridWidth != gridWidth - 1)
				nodes.at(i)->addNeighbour(nodes.at(i - gridWidth + 1)); //upper right
		}
		if (i % gridWidth != gridWidth - 1)
		{
			nodes.at(i)->addNeighbour(nodes.at(i + 1)); //Right
			if (i + gridWidth < gridWidth * gridHeight)
			{
				nodes.at(i)->addNeighbour(nodes.at(i + gridWidth + 1)); //Lower right
			}
		}
		if (i + gridWidth < gridWidth * gridHeight)
		{
			nodes.at(i)->addNeighbour(nodes.at(i + gridWidth)); //Lower mid
		}

	}
}
int AStar::getDistance(Vector2 pos1, Vector2 pos2)
{
	return int((pos1 - pos2).Length() * 10 );	
}

void AStar::addToVector(std::vector<Node*>& nodes, Node* nodeToAdd)
{
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodeToAdd->getFCost() >= nodes.at(i)->getFCost())
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
	while (current->getPreviousNode() != nullptr)
	{
		path.push_back(current);
		current = current->getPreviousNode();
	}
}

void AStar::resetNodes()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes.at(i)->reset();
	}
}
