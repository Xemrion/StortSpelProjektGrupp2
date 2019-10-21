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
		short int   gCost = 0, fCost = 0, prevIndex = -1;
		bool  isTraversible;
	};
private:
	int getDistance(Vector3 pos1, Vector3 pos2);
	void addToVector(std::vector<short>& nodes, short nodeToAdd);
	bool isInVector(std::vector<short> closed, short node);
	void reconstructPath(short goal, std::vector<Vector3>& path);
	void resetTileData();
	TileMap* map;
	std::vector<TileData> tileData;
	std::vector<short> changedTiles;
public:
	void generateTileData(TileMap const& map);
	bool algorithm(Vector3 startPos, Vector3 endPos, std::vector<Vector3>& path);
	AStar();
	~AStar();
	AStar(TileMap* map);

};
