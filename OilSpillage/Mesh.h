#pragma once
#include"VertexBuffer.h"
#include"Structs.h"
#include<vector>
using namespace DirectX::SimpleMath;

class Mesh
{
public:
	Mesh();
	void insertDataToMesh(std::vector<Vertex3D> vertexData);
	void operator=(const Mesh& rh) { this->vertices = rh.vertices; }

	virtual ~Mesh() { }
	std::vector<Vertex3D> vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	void setAABB(AABB obj);
	AABB getAABB() const;
	int getVertexCount() const;
private:
	AABB collisionBox;
	int vertexCount = 0; 
};
