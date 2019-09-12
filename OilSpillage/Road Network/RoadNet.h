#ifndef ROADNET_H
#define ROADNET_H
#include<DirectXMath.h>
#include"..//GameObject.h"

struct Segment {
	glm::fvec4 firstPoint,
		secondPoint;
	Segment* crossing = nullptr;
	glm::fvec3* crossingPosition = nullptr;

	~Segment() {
		if (crossingPosition != nullptr) {
			delete this->crossingPosition;
		}
	}
};

class RoadNetwork
{
public:
	RoadNetwork(int randNr, glm::fvec2 max, glm::fvec2 min, float rotation = 90);
	~RoadNetwork();

	void setLetters(char forward = 'F', char left = '-', char right = '+');
	void setAngle(float angle);
	float getAngle() const;
	void clearSegments();
	bool generateInitialSegments(const char* seed);
	bool generateAdditionalSegments(const char* seed);


private:
	glm::fvec4 startPoint, endPoint;
	glm::fvec4 forward;
	glm::fvec2 roadNetworkMax, roadNetworkMin;
	float rotationAngle, forwardDistance;
	std::vector<Segment> roadNetwork;
	char forwardSymbol, turnLeftSymbol, turnRightSymbol;
};

#endif // !ROADNET_H
