#include "AStar.h"


bool AStar::algorithm(Vector3 startPos, Vector3 endPos, std::vector<Vector3>& path)
{
	if (!map.isInBounds(map.convertWorldPositionToTilePositionXZ(startPos)) || !map.isInBounds(map.convertWorldPositionToTilePositionXZ(endPos)))
	{
		return false;
	}
	//startPos = Vector3(startPos.x, startPos.y, -startPos.z);
	//endPos = Vector3(endPos.x, endPos.y, -endPos.z);
	Vector3 startTilePos = Vector3(map.convertWorldPositionToTilePositionXZ(startPos).x,
		0,
		map.convertWorldPositionToTilePositionXZ(startPos).y);
	Vector3 goalTilePos = Vector3(map.convertWorldPositionToTilePositionXZ(endPos).x,
		0,
		map.convertWorldPositionToTilePositionXZ(endPos).y);
	path.clear();
	resetTileData();
	Size startIndex = map.index(startTilePos.x, startTilePos.z);
	Size goalIndex = map.index(goalTilePos.x, goalTilePos.z);
	if (!tileData[goalIndex].isTraversible)
	{
		return false;
	}
	std::vector<Size> open;
	std::vector<Size> closed;
	Size current;
	open.push_back(startIndex);
	std::vector<Size> neighbours;
	Size curNeighbour;
	while (!open.empty())
	{
		current = open.at(open.size() - 1);
		if (current == goalIndex)
		{
			reconstructPath(goalIndex, path);
			return true;
		}
		open.pop_back();
		closed.push_back(current);
		neighbours = map.getNeighbouringIndices(map.getTilePosByIndex(current));
		for (int i = 0; i < neighbours.size(); i++)
		{
			curNeighbour = neighbours.at(i);
			//If it is traversable and isn't in closed
			if (tileData.at(curNeighbour).isTraversible && !isInVector(closed, curNeighbour))
			{
				//Calculate new path
				Size newGCost = tileData.at(curNeighbour).gCost + getDistance(map.getWorldPosByIndex(curNeighbour), map.getWorldPosByIndex(current));
				Size newHCost = getDistance(map.getWorldPosByIndex(curNeighbour), map.getWorldPosByIndex(goalIndex));
				Size newFCost = newHCost + newGCost;

				if (!isInVector(open, curNeighbour) || newFCost < tileData.at(curNeighbour).fCost)
				{
					tileData.at(curNeighbour).gCost = newGCost;
					tileData.at(curNeighbour).fCost = newFCost;
					tileData.at(curNeighbour).prevIndex = current;
					changedTiles.push_back(curNeighbour);
					if (!isInVector(open, curNeighbour))
					{
						addToVector(open, curNeighbour);
					}

				}
			}
		}

	}
	return false;
}

AStar::~AStar()
{
}
AStar::AStar(TileMap const& map) :
	map(map)
{
}
int AStar::getDistance(Vector3 pos1, Vector3 pos2)
{
	return int((pos2 - pos1).Length()) * 10;
}

void AStar::addToVector(std::vector<Size>& nodes, Size nodeToAdd)
{
	for (int i = 0; i < nodes.size(); i++)
	{
		if (tileData.at(nodeToAdd).fCost >= tileData.at(nodes.at(i)).fCost)
		{
			nodes.insert(nodes.begin() + i, nodeToAdd);
			return;
		}
	}
	nodes.push_back(nodeToAdd);
}

bool AStar::isInVector(std::vector<Size>& vector, Size node)
{
	for (int i = 0; i < vector.size(); i++)
	{
		if (vector.at(i) == node)
		{
			return true;
		}
	}
	return false;
}

void AStar::reconstructPath(Size goalIndex, std::vector<Vector3>& path)
{
	Size current;
	current = goalIndex;
	while (tileData.at(current).prevIndex != -1)
	{
		path.emplace_back(map.getWorldPosByIndex(current));
		current = tileData.at(current).prevIndex;
	}
}

void AStar::resetTileData()
{
	for (int i = 0; i < changedTiles.size(); i++)
	{
		tileData.at(changedTiles.at(i)).fCost = 0;
		tileData.at(changedTiles.at(i)).gCost = 0;
		tileData.at(changedTiles.at(i)).prevIndex = -1;
	}
	changedTiles.clear();
}

void AStar::generateTileData(TileMap const& map)
{
	tileData = std::vector<TileData>{ map.width * map.height };
	for (auto i = 0; i < map.data.size(); ++i)
	{
		if (map.data[i] == Tile::building)
		{
			tileData[i].isTraversible = false;
		}
	}
}