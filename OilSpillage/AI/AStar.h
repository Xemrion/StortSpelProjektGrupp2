#pragma once
#include<vector>
#include<d3d11.h>
#include<SimpleMath.h>
#include"Node.h"
#include "../PG/Tile.hpp"
#include "../PG/TileMap.hpp"
using namespace DirectX::SimpleMath;
class AStar
{
	struct TileData 
	{
		Size  gCost = 0, fCost = 0, prevIndex = -1;
		bool  isTraversible;
	};
private:
	int getDistance(Vector3 pos1, Vector3 pos2);
	void addToVector(std::vector<Size>& nodes, Size nodeToAdd);
	bool isInVector(std::vector<Size> closed, Size node);
	void reconstructPath(Size goal, std::vector<Vector3>& path);
	void resetTileData();
	TileMap const &map;
	std::vector<TileData> tileData;
	std::vector<Size> changedTiles;
public:
	void generateTileData(TileMap const& map);
	bool algorithm(Vector3 startPos, Vector3 endPos, std::vector<Vector3>& path);
	~AStar();
	AStar(TileMap const &map);

};
