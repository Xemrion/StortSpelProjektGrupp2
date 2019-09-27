#include"Mesh.h"
Mesh::Mesh()
{

};

void Mesh::loadMesh(const char* filename) 
{
	Vertex3D vertex;
	vertex.position = Vector3(-1.0, 0.0, -1.0);
	vertex.uv = Vector2(0.0, 1.0);
	vertex.normal = Vector3(0.0, 1.0, 0.0);
	vertices.push_back(vertex);

	vertex.position = Vector3(-1.0, 0.0, 1.0);
	vertex.uv = Vector2(0.0, 0.0);
	vertices.push_back(vertex);

	vertex.position = Vector3(1.0, 0.0, -1.0);
	vertex.uv = Vector2(1.0, 1.0);
	vertices.push_back(vertex);

	vertex.position = Vector3(-1.0, 0.0, 1.0);
	vertex.uv = Vector2(0.0, 0.0);
	vertices.push_back(vertex);

	vertex.position = Vector3(1.0, 0.0, 1.0);
	vertex.uv = Vector2(1.0, 0.0);
	vertices.push_back(vertex);

	vertex.position = Vector3(1.0, 0.0, -1.0);
	vertex.uv = Vector2(1.0, 1.0);
	vertices.push_back(vertex);

	this->vertexCount = static_cast<int>(vertices.size());

};
void Mesh::insertDataToMesh(std::vector<Vertex3D>& vertexData)
{
	this->vertices = vertexData;

	this->vertexCount = static_cast<int>(vertices.size());
};
void Mesh::setAABB(AABB obj)
{
	this->collisionBox = obj;
}
AABB Mesh::getAABB()const
{
	return this->collisionBox;
}
int Mesh::getVertexCount() const
{

	return this->vertexCount;
};