#pragma once
#include"VertexBuffer.h"
#include"Structs.h"
#include<vector>
class Mesh
{
public:
	Mesh();
	virtual~Mesh();
	void loadMesh(char* filename);
private:
	std::vector<Vertex3D> mesh;//Make our own vector later
	VertexBuffer<Vertex3D> vertexBuffer;
	
};