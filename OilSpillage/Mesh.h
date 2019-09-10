#pragma once
#include"VertexBuffer.h"
#include"Structs.h"
#include<vector>

class Mesh
{
public:
	void loadMesh(const char* filename) {
		Vertex3D vertex;
		vertex.position = glm::vec3(-1.0, 0.0 ,- 1.0);
		vertex.uv = glm::vec2(0.0, 0.0);
		vertex.normal = glm::vec3(0.0, 1.0, 0.0);
		vertices.push_back(vertex);

		vertex.position = glm::vec3(-1.0, 0.0, 1.0);
		vertices.push_back(vertex);

		vertex.position = glm::vec3(1.0, 0.0, -1.0);
		vertices.push_back(vertex);

		vertex.position = glm::vec3(-1.0, 0.0, 1.0);
		vertices.push_back(vertex);

		vertex.position = glm::vec3(1.0, 0.0, 1.0);
		vertices.push_back(vertex);

		vertex.position = glm::vec3(1.0, 0.0, -1.0);
		vertices.push_back(vertex);

		this->vertexCount = vertices.size();
		
	};
	void operator=(const Mesh& rh) { this->vertices = rh.vertices; }
	std::vector<Vertex3D> vertices;
	ID3D11Buffer* vertexBuffer;
	int getVertexCount() const 
	{
		return this->vertexCount;
	};
private:
	int vertexCount; 
};
