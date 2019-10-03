#include "SkyscraperFloor.h"

SkyscraperFloor::SkyscraperFloor(int edges)
{
	this->center = Vector3(0.0f, 0.0f, 0.0f);
	this->verticies.clear();
	this->generateShape(edges);
}

SkyscraperFloor::SkyscraperFloor(const SkyscraperFloor& other)
{
}

SkyscraperFloor::~SkyscraperFloor()
{
}
void SkyscraperFloor::rotateDeg(float degrees)
{
	float rotX = 0.0f, rotZ = 0.0f;
	float angle = degrees * (pi / 180.0f);
	for (int i = 0; i < this->verticies.size(); i++) {
		rotX = (this->verticies[i].x * float(cos(angle))) + (this->verticies[i].z * float(sin(angle)));
		rotZ = (this->verticies[i].z * float(cos(angle))) - (this->verticies[i].x * float(sin(angle)));
		this->verticies[i].x = rotX;
		this->verticies[i].z = rotZ;
	}
}
void SkyscraperFloor::rotateRad(float radians)
{
	float rotX = 0.0f, rotZ = 0.0f;
	float angle = radians;
	for (int i = 0; i < this->verticies.size(); i++) {
		rotX = (this->verticies[i].x * float(cos(angle))) + (this->verticies[i].z * float(sin(angle)));
		rotZ = (this->verticies[i].z * float(cos(angle))) - (this->verticies[i].x * float(sin(angle)));
		this->verticies[i].x = rotX;
		this->verticies[i].z = rotZ;
	}
}
/*
nextX = (currentForward.x * float((cos(rotationAngle)))) + (currentForward.z * float(sin(rotationAngle)));
nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
*/
void SkyscraperFloor::generateShape(int edges)
{
	this->nrOfEdges = edges;
	Vector3 temp = this->center + Vector3(0.0f, 0.0f, 1.0f);
	float angle = ((pi * 2) / edges);
	this->verticies.push_back(temp);
	for (int i = 1; i < edges; i++) {
		float rotX = (temp.x * float(cos(angle))) + (temp.z * float(sin(angle)));
		float rotZ = (temp.z * float(cos(angle))) - (temp.x * float(sin(angle)));
		temp = Vector3(rotX, 0.0f, rotZ);
		this->verticies.push_back(temp);
	}
}

void SkyscraperFloor::unionShapes(SkyscraperFloor& toUnion)
{
}

void SkyscraperFloor::translate(Vector3 newCenter)
{
}
