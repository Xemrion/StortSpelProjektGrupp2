#include "SkyscraperFloor.h"

SkyscraperFloor::SkyscraperFloor(int edges)
{
	this->center = Vector3(0.0f, 0.0f, 0.0f);
	this->verticies.clear();
	this->nrOfEdges = 0;
	this->generateShape(edges);
}

SkyscraperFloor::SkyscraperFloor(const SkyscraperFloor& other)
{
	this->center = other.center;
	this->verticies.clear();
	this->verticies = other.verticies;
	this->nrOfEdges = other.nrOfEdges;
	this->indices.clear();
	this->indices = other.indices;
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

void SkyscraperFloor::generateShape(int edges)
{
	if (edges >= 3) {
		this->nrOfEdges = edges;
		Vector3 temp = this->center + Vector3(0.0f, 0.0f, 3.0f);
		float angle = ((pi * 2) / float(edges));
		this->verticies.push_back(temp);
		for (int i = 1; i < edges; i++) {
			float rotX = (temp.x * float(cos(angle))) + (temp.z * float(sin(angle)));
			float rotZ = (temp.z * float(cos(angle))) - (temp.x * float(sin(angle)));
			temp = Vector3(rotX, temp.y, rotZ);
			this->verticies.push_back(temp);
		}
	}
}

Vector3 SkyscraperFloor::intersectingLines(Vector3& pointA1, Vector3& pointA2, Vector3& pointB1, Vector3& pointB2) const
{
	float divider, t1, t2;
	Vector3 result(0.0f, -1.0f, 0.0f);

	divider = (((pointB2.x - pointB1.x) * (pointA1.z - pointA2.z))
		- ((pointA1.x - pointA2.x) * (pointB2.z - pointB1.z)));

	if (divider != 0.0f) {

		t1 = (((pointB1.z - pointB2.z) * (pointA1.x - pointB1.x)) +
			((pointB2.x - pointB1.x) * (pointA1.z - pointB1.z)))
			/ divider;

		t2 = (((pointA1.z - pointA2.z) * (pointA1.x - pointB1.x)) +
			((pointA2.x - pointA1.x) * (pointA1.z - pointB1.z)))
			/ divider;

		if (t1 > 0 && t2 > 0 && t1 <= 1 && t2 <= 1) {
			result = Vector3(pointA1.x + (pointA2.x - pointA1.x) * t1, pointA1.y, pointA1.z + (pointA2.z - pointA1.z) * t1);
		}
	}
	return result;
}

bool SkyscraperFloor::evenOddCheck( Vector3 pointB1,Vector3 pointB2, Vector3 pointA) const
{
	bool intersect = false;
	
	float divider, t1, t2;
	Vector3 direction(1.0f, 0.0f, 1.0f);

	divider = (((pointB2.x - pointB1.x) * (pointA.z - direction.z))
		- ((pointA.x - direction.x) * (pointB2.z - pointB1.z)));

	if (divider != 0) {

		t1 = (((pointB1.z - pointB2.z) * (pointA.x - pointB1.x)) +
			((pointB2.x - pointB1.x) * (pointA.z - pointB1.z)))
			/ divider;

		t2 = (((pointA.z - direction.z) * (pointA.x - pointB1.x)) +
			((direction.x - pointA.x) * (pointA.z - pointB1.z)))
			/ divider;

		if (t1 > 0 && t2 > 0 && t2 <= 1) {
			intersect = true;
		}
	}
	
	return intersect;
}

void SkyscraperFloor::unionShapes(SkyscraperFloor& toUnion, Vector3 newCenter)
{
	/* Union
	This Shape A
	Other Shape B
	Go through lines in A and see if they intersect with lines from B
	If they intersect, add a point between the lines in A & B.
	After all lines are done, compare points in A to Shape B
	If inside, remove from Copy A
	Repeat for B
	Add points of B between the intersecting points added to A
	You have a union.
	*/

	//Phase 1: Intersections.
	toUnion.translate(newCenter);
	std::vector<Vector3> intersections;
	Vector3 intersectionA(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < toUnion.nrOfEdges; i++) {
		for (int j = 0; j < this->nrOfEdges; j++) {
			intersectionA = intersectingLines(toUnion.verticies[i], toUnion.verticies[(size_t(i) + 1) % toUnion.nrOfEdges], this->verticies[j], this->verticies[(size_t(j) + 1) % this->nrOfEdges]);
			if (intersectionA.y != -1.0f) {
				intersections.push_back(intersectionA);
				if (this->verticies[(size_t(j) + 1) % this->nrOfEdges] != intersectionA) {
					this->verticies.insert(this->verticies.begin() + j + 1, intersectionA);
					this->nrOfEdges++;
				}
				if (toUnion.verticies[(size_t(i) + 1) % toUnion.nrOfEdges] != intersectionA) {
					toUnion.verticies.insert(toUnion.verticies.begin() + i + 1, intersectionA);
					toUnion.nrOfEdges++;
				}
			}
		}
	}
	if (intersections.size() > 0) {
		//Even-Odd Rule
		int evenOddCount;
		bool found;
		//Remove points with an odd number of intersections with the other shape. (Odd = inside, Even = outside)
		SkyscraperFloor temp1(toUnion);
		SkyscraperFloor temp2(*this);
		for (int i = 0; i < this->nrOfEdges; i++) { //Check points in this
			found = false;
			evenOddCount = 0;
			for (int k = 0; k < intersections.size(); k++) {
				if (this->verticies[i] == intersections[k]) {
					found = true;
				}
			}
			if (!found) {
				for (int j = 0; j < temp1.nrOfEdges; j++) {
					if (evenOddCheck(temp1.verticies[j], temp1.verticies[(size_t(j) + 1) % toUnion.nrOfEdges], this->verticies[i])) {
						evenOddCount++;
					}
				}
				if (evenOddCount % 2 == 1) {
					this->verticies.erase(this->verticies.begin() + i);
					this->nrOfEdges--;
					i--;
				}
			}
		}
		for (int i = 0; i < toUnion.nrOfEdges; i++) { //Check points in toUnion
			found = false;
			evenOddCount = 0;
			for (int k = 0; k < intersections.size() && !found; k++) {
				if (toUnion.verticies[i] == intersections[k]) {
					found = true;
				}
			}
			if (!found) {
				for (int j = 0; j < temp2.nrOfEdges; j++) {
					if (evenOddCheck(temp2.verticies[j], temp2.verticies[(size_t(j) + 1) % temp2.nrOfEdges], toUnion.verticies[i])) {
						evenOddCount++;
					}
				}
				if (evenOddCount % 2 == 1) {
					toUnion.verticies.erase(toUnion.verticies.begin() + i);
					toUnion.nrOfEdges--;
					i--;
				}
			}
		}

		//Add them together
		std::vector<std::pair<int,int>> indicesOfIntersections;
		indicesOfIntersections.resize(intersections.size());
		for (int i = 0; i < intersections.size(); i++) { 
			for (int j = 0; j < toUnion.nrOfEdges; j++) {
				if (intersections[i] == toUnion.verticies[j]) {
					indicesOfIntersections[i].first = j; //Save index of intersection to other shape
				}
			}
			for (int j = 0; j < this->nrOfEdges; j++) {
				if (intersections[i] == this->verticies[j]) {
					indicesOfIntersections[i].second = j; //Save index of intersection to this shape
				}
			}
		}
		if (indicesOfIntersections[0].first == 0) { //If first is 0, then it is connected to second
			for (int i = 0; i < indicesOfIntersections.size(); i+=2) {
				this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first + 1,
						toUnion.verticies.begin() + size_t(indicesOfIntersections[size_t(i) + 1].first));
			}
		}
		else { //If first is not 0, then first and last and connected.
			for (int i = 1; i < indicesOfIntersections.size(); i+=2) {
				if (i == indicesOfIntersections.size() - 1) {
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first + 1,
						toUnion.verticies.end());
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1 + ((size_t(toUnion.nrOfEdges) - 1) - (indicesOfIntersections[i].first)),
						toUnion.verticies.begin(),
						toUnion.verticies.begin() + indicesOfIntersections[0].first);
				}
				else {
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first,
						toUnion.verticies.begin() + (size_t(indicesOfIntersections[size_t(i) + 1].first) - 1));
				}
			}
		}
		this->nrOfEdges = int(this->verticies.size());

	}
	else {
		//Even-Odd Rule on one point in 
		int evenOddCount = 0;
		for (int i = 0; i < toUnion.nrOfEdges; i++) {
			evenOddCount = 0;
			if (i == toUnion.nrOfEdges - 1) {
				if (evenOddCheck(toUnion.verticies[i], toUnion.verticies[0], this->verticies[0])) {
					evenOddCount++;
				}
			}
			else {
				if (evenOddCheck(toUnion.verticies[i], toUnion.verticies[size_t(i) + 1], this->verticies[0])) {
					evenOddCount++;
				}
			}
		}if (evenOddCount % 2 == 1) {
			this->verticies.clear();
			this->verticies = toUnion.verticies;
		}
	}
}

void SkyscraperFloor::testDrawLines() const
{
	
	for (int i = 0; i < this->indices.size(); i += 3) {
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[i]].x, this->center.y, this->verticies[this->indices[i]].z),
			XMFLOAT3(this->verticies[this->indices[(size_t(i) + 1)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 1)]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[(size_t(i) + 1)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 1)]].z),
			XMFLOAT3(this->verticies[this->indices[(size_t(i) + 2)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 2)]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[(size_t(i) + 2)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 2)]].z),
			XMFLOAT3(this->verticies[this->indices[i]].x, this->center.y, this->verticies[this->indices[i]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
	}
}

void SkyscraperFloor::getTriangleIndices()
{ //Bug: Infinite Loop
	this->indices.clear();
	std::vector<int> triangleIndices;
	std::vector<bool> usedIndices;
	size_t counter = 1, forwardsOffset = 1, backwardsOffset = 1;
	size_t size = this->verticies.size();
	bool counterUsed = false, forwardsUsed = false, backwardsUsed = false;
	bool mainPointLeft = false;
	int pointsOnMainPointSide = 0;
	int pointsUsed = 0;
	usedIndices.resize(size);

	while (size - pointsUsed > 2) {
		counterUsed = false;
		if (usedIndices[counter]) {
			counterUsed = true;
		}
		if (!counterUsed) {
			Vector3 backToFront, backToOther, crossProduct;
			bool towardsUsed = false;
			//Check forward until an available is found, save offset from point
			do {
				forwardsUsed = false;
				if (usedIndices[(size_t(counter) + size_t(forwardsOffset)) % size]) {
					forwardsUsed = true;
				}
				if (forwardsUsed) {
					forwardsOffset++;
				}
			} while (forwardsUsed);

			//Check backward until an available is found, save offset from point
			do {
				backwardsUsed = false;
				if (usedIndices[(counter - backwardsOffset + size) % size]) {
					backwardsUsed = true;
				}
				if (backwardsUsed) {
					backwardsOffset++;
					if (backwardsOffset >= size) {
						backwardsOffset -= size;
					}
				}
			} while (backwardsUsed);

			if (forwardsOffset !=  size && backwardsOffset != size) {
				/**/
				
				//Make a line from backward to forward and backward to main point
				backToFront = this->verticies[(counter + forwardsOffset) % size] - this->verticies[(counter - backwardsOffset + size) % size];
				backToOther = this->verticies[counter] - this->verticies[(counter - backwardsOffset + size) % size];
				backToFront.Normalize();
				backToOther.Normalize();
				//Cross product the start point line first
				crossProduct = backToFront.Cross(backToOther);
				if (crossProduct.y > 0) {
					mainPointLeft = false;
					pointsOnMainPointSide++;
				}
				else if (crossProduct.y < 0) {
					mainPointLeft = true;
					pointsOnMainPointSide++;
				}
				//Cross product the start point line first, then others, if any are on the same side as the point line, there cannot be a triangle
				for (size_t i = 1 + counter + forwardsOffset; i < size + ((counter - backwardsOffset + size) % size) && pointsOnMainPointSide < 2; i++) {
					towardsUsed = false;
					if (usedIndices[i % size]) {
						towardsUsed = true;
					}
					if (!towardsUsed) {
						backToOther = this->verticies[i % size] - this->verticies[(counter - backwardsOffset + size) % size];
						backToOther.Normalize();
						crossProduct = backToFront.Cross(backToOther);
						if (!mainPointLeft) {
							if (crossProduct.y > 0) {
								pointsOnMainPointSide++;
							}
						}
						else {
							if (crossProduct.y < 0) {
								pointsOnMainPointSide++;
							}
						}
					}
				}
				if (pointsOnMainPointSide < 2) {
					triangleIndices.push_back(int(counter - backwardsOffset + int(size)) % int(size));
					triangleIndices.push_back(int(counter));
					triangleIndices.push_back(int(counter + forwardsOffset) % int(size));
					usedIndices[counter] = true;
					pointsUsed++;
				}

				pointsOnMainPointSide = 0;
			}

		}
		counter++;
		counter = counter % size;

		backwardsOffset = 1;
		forwardsOffset = 1;
	}
	this->indices.clear();
	this->indices = triangleIndices;
}

std::vector<Vertex3D> SkyscraperFloor::getWallVertices(Vector3 otherCenter)
{	//Strange triangles
	std::vector<Vertex3D> meshData;
	Vertex3D temp;
	Vector2 topLeft(0, 0), topRight(1, 0), botLeft(0, 1), botRight(1, 1);
	int counter = 0;
	float counterPow = 0.0f, distSquared = 0.0f;
	Vector3 lowerFirst(0.0f), lowerSecond(0.0f);
	Vector3 normal(0.0f), line1(0.0f), line2(0.0f);
	for (size_t i = 0; i < this->verticies.size(); i++) {
		lowerFirst = Vector3(this->verticies[i].x, otherCenter.y, this->verticies[i].z);
		lowerSecond = Vector3(this->verticies[(i + 1) % this->nrOfEdges].x, otherCenter.y, this->verticies[(i + 1) % this->nrOfEdges].z);
	
		line1 = this->verticies[(i + 1) % this->nrOfEdges] - this->verticies[i];
		line2 = lowerSecond - this->verticies[i];
		normal = line2.Cross(line1);

		temp.position = this->verticies[i];
		temp.normal = normal;
		temp.uv = topRight;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = lowerFirst;
		temp.normal = normal;
		temp.uv = botRight;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = lowerSecond;
		temp.normal = normal;
		temp.uv = botLeft;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = this->verticies[(i + 1) % this->nrOfEdges];
		temp.normal = normal;
		temp.uv = topLeft;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = this->verticies[i];
		temp.normal = normal;
		temp.uv = topRight;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = lowerSecond;
		temp.normal = normal;
		temp.uv = botLeft;
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

	}
	return meshData;
}

std::vector<Vertex3D> SkyscraperFloor::getRoofVertices()
{
	std::vector<Vertex3D> meshData;
	Vertex3D temp;
	//float minUV = 1.0f / 24.0f * 3.0f, maxUV = 1.0f / 24.0f * 6.0f;
	Vector3 normal(0.0f), line1(0.0f), line2(0.0f);
	if (this->indices.size() == 0) {
		this->getTriangleIndices();
	}
	for (size_t i = 0; i < this->indices.size(); i += 3) {
		line1 = this->verticies[this->indices[i + 1]] - this->verticies[this->indices[i]];
		line2 = this->verticies[this->indices[i + 2]] - this->verticies[this->indices[i]];
		normal = line1.Cross(line2);

		temp.position = this->verticies[this->indices[i]];
		temp.normal = normal;
		temp.uv = Vector2(temp.position.x, 0.2);
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = this->verticies[this->indices[size_t(i) + 1]];
		temp.normal = normal;
		temp.uv = Vector2(temp.position.x, 0);
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);

		temp.position = this->verticies[this->indices[size_t(i) + 2]];
		temp.normal = normal;
		temp.uv = Vector2(temp.position.x,0.2);
		temp.tangent = temp.normal;
		temp.bitangent = temp.normal;
		meshData.push_back(temp);
	}
	return meshData;
}

void SkyscraperFloor::testDrawTriangles(std::string name, Vector4 colour)
{
	std::vector<Vertex3D> meshData;
	meshData = this->getRoofVertices();
	this->roof = new GameObject;
	Game::getGraphics().loadMesh(name.c_str(), meshData);
	this->roof->mesh = Game::getGraphics().getPGMeshPointer(name.c_str());
	Game::getGraphics().addToDraw(this->roof);
	this->roof->setPosition(this->center);
	this->roof->setScale(Vector3(2.50f, 1.0f, 2.50f));
	this->roof->setColor(colour);
}

Vector3 SkyscraperFloor::getAVertex(int vertex) const //Returns the vertex in the index - 1
{
	Vector3 returnVertex;
	returnVertex = this->verticies[(size_t(vertex) - 1) % this->nrOfEdges];
	return returnVertex;
}

Vector3 SkyscraperFloor::getCenter() const
{
	return this->center;
}

int SkyscraperFloor::getNrOfEdges() const
{
	return this->nrOfEdges;
}

void SkyscraperFloor::translate(Vector3 newCenter)
{
	Vector3 translation = newCenter - this->center;
	this->center = this->center + translation;
	for (int i = 0; i < this->nrOfEdges; i++) {
		this->verticies[i] = this->verticies[i] + translation;
	}
	
}

void SkyscraperFloor::translateBy(Vector3 moveBy)
{
	Vector3 translation = moveBy;
	this->center = this->center + translation;
	for (int i = 0; i < this->nrOfEdges; i++) {
		this->verticies[i] = this->verticies[i] + translation;
	}

}

void SkyscraperFloor::scale(Vector3 scaleBy)
{
	Vector3 newPoint(0.0f);
	for (int i = 0; i < this->nrOfEdges; i++) {
		newPoint = this->verticies[i] - this->center;
		newPoint *= scaleBy;
		this->verticies[i] = newPoint;
	}
}

void SkyscraperFloor::regenerateShape(int edges)
{
	this->verticies.clear();
	this->center = Vector3(0.0f, 0.0f, 0.0f);
	this->nrOfEdges = 0;
	generateShape(edges);
}
