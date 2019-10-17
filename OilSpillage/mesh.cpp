#include"Mesh.h"
Mesh::Mesh()
{

};

void Mesh::insertDataToMesh(std::vector<Vertex3D> vertexData)
{
	this->vertices = vertexData;

	this->vertexCount = static_cast<int>(vertices.size());
};

void Mesh::setAABB(AABB obj)
{
	this->collisionBox = obj;
}

AABB Mesh::getAABB() const
{
	return this->collisionBox;
}

int Mesh::getVertexCount() const
{
	return this->vertexCount;
};