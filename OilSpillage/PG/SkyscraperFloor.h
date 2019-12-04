#pragma once
#include<d3d11.h>
#include<SimpleMath.h>
#include<vector>
#include"..//Structs.h"
#include"..//game.h"
using namespace DirectX::SimpleMath;

class SkyscraperFloor
{
public:
	SkyscraperFloor(int edges = 4);
	SkyscraperFloor(const SkyscraperFloor& other);
	~SkyscraperFloor();

	void rotateDeg(float degrees);
	void rotateRad(float radians);
	void unionShapes(SkyscraperFloor& toUnion, Vector3 newCenter);
	Vector3 getAVertex(int vertex) const;
	Vector3 getCenter() const;
	int getNrOfEdges() const;
	void translate(Vector3 newCenter);
	void translateBy(Vector3 moveBy);
	void scale(Vector3 scaleBy);
	void regenerateShape(int edges);

	bool getTriangleIndices();
	std::vector<Vertex3D> getWindowVertices(Vector3 otherCenter);
	std::vector<Vertex3D> getWallVertices(Vector3 otherCenter);
	std::vector<Vertex3D> getRoofVertices();
	std::vector<Vertex3D> getDifferenceAsRoofVerticies(const SkyscraperFloor& other);
private:
	std::vector<Vector3> verticies;
	std::vector<int> indices;
	int nrOfEdges;
	bool triangleGenFail = false;
	Vector3 center;
	GameObject* roof;

	float pi = 3.141592653f;


	void generateShape(int edges);
	Vector3 intersectingLines(Vector3 pointA1, Vector3 pointA2, Vector3 pointB1, Vector3 pointB2) const;
	bool evenOddCheck(Vector3 pointB1, Vector3 pointB2, Vector3 pointA) const;
	bool evenOddCheck(Vector3 pointB1, Vector3 pointB2, Vector3 pointA, Vector3 pointTowards) const;
};
