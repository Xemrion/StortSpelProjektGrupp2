#include "AStar.h"


std::vector<Node*> AStar::algorithm(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 endPos)
{
	Node* start = getNode(startPos);
	Node* goal = getNode(endPos);
	std::vector<Node*> open;
	std::vector<Node*> closed;
	Node* current;
	open.push_back(start);
	Node* neighbour;
	while (!open.empty())
	{
		//PrintNodes();
		current = open.at(open.size() - 1);
		if (current == goal)
			return reconstructPath(goal);

		open.pop_back();
		closed.push_back(current);
		for (int i = 0; i < current->GetNeighbours().size(); i++)
		{
			neighbour = current->GetNeighbours().at(i);
			//If it is traversable and isn't in closed
			if (neighbour->IsTraversable() && !isInVector(closed, neighbour))
			{
				//Calculate new path
				int newFCost = getDistance(neighbour->GetPos(), goal->GetPos()) + getDistance(neighbour->GetPos(), start->GetPos());

				if (!isInVector(open, neighbour) || newFCost < neighbour->GetFCost())
				{
					//G and H For debugging
					neighbour->SetGCost(getDistance(neighbour->GetPos(), start->GetPos()));
					neighbour->SetHCost(getDistance(neighbour->GetPos(), goal->GetPos()));
					neighbour->SetFCost(neighbour->GetGCost() + neighbour->GetHCost());
					neighbour->SetPreviousNode(current);
					if (!isInVector(open, neighbour))
						addToVector(open, neighbour);

				}
			}
		}

	}
	std::vector<Node*> failed;
	return failed;
}

Node* AStar::getNode(DirectX::SimpleMath::Vector3 position)
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
AStar::AStar(int gridWidth, int gridHeight)
{
	//Create grid
	this->gridWidth = gridWidth;
	this->gridHeight = gridHeight;
	for (int i = 0; i < gridWidth * gridHeight; i++)
	{
		Node* temp = new Node();
		temp->SetID(i);
		temp->SetXPos(i % gridWidth);
		temp->SetYPos(i / gridWidth);
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
		if (i - gridWidth > 0)
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
				nodes.at(i)->AddNeighbour(nodes.at(i + gridWidth)); //Lower mid
			}
		}

	}
}
int AStar::getDistance(DirectX::SimpleMath::Vector2 pos1, DirectX::SimpleMath::Vector2 pos2)
{
	int x1 = pos1.x - pos2.x;
	int y1 = pos1.y - pos2.y;
	return sqrt(pow(x1, 2) + pow(y1, 2)) * 10;
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

std::vector<Node*> AStar::reconstructPath(Node* goal)
{
	std::vector<Node*> path;
	Node* current;
	current = goal;
	while (current->GetPreviousNode() != nullptr)
	{
		path.push_back(current);
		current = current->GetPreviousNode();
	}
	return path;
}