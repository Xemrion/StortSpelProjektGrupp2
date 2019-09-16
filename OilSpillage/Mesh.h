#pragma once
#include"VertexBuffer.h"
#include"Structs.h"
#include<vector>
using namespace DirectX::SimpleMath;
struct AABB
{
	Vector3 maxPos;
	Vector3 minPos;
};

class Mesh
{
public:
	Mesh();
	void loadMesh(const char* filename);
	void insertDataToMesh(std::vector<Vertex3D>& vertexData);
	void operator=(const Mesh& rh) { this->vertices = rh.vertices; }

	virtual ~Mesh() { if (this->vertexBuffer) this->vertexBuffer->Release(); }
	std::vector<Vertex3D> vertices;
	ID3D11Buffer* vertexBuffer = nullptr;
	void setAABB(AABB obj);
	AABB getAABB()const;
	int getVertexCount()const;
private:
	AABB collisionBox;
	int vertexCount = 0; 
};
