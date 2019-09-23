#ifndef ROADNET_H
#define ROADNET_H
#include<string>
#include<vector>
#include<fstream>
#include<d3d11.h>
#include<DirectXMath.h>
#include<SimpleMath.h>
using namespace DirectX::SimpleMath;

struct Segment {
	Vector4 firstPoint,
		secondPoint;
	Segment* crossing = nullptr;
	Vector3* crossingPosition = nullptr;
	~Segment() {
		if (crossingPosition != nullptr) {
			delete this->crossingPosition;
		}
	}
};
struct SegmentVertex {
	Vector3 point,
		colour;
};
class RoadNetwork
{
public:
	RoadNetwork(int randNr, Vector2 max, Vector2 min, float rotation = 90);
	~RoadNetwork();

	void setLetters(char forward = 'F', char left = '-', char right = '+', char half = 'H');
	void setAngle(float angle);
	float getAngle() const;
	void clearSegments();
	bool generateInitialSegments(const char* seed);
	bool generateAdditionalSegments(const char* seed, int segment);
	void cleanRoadNetwork();
	
	bool saveTestNetwork(std::string filename);
	bool loadTestNetwork(std::string filename);
private:
	Vector4 startPoint, endPoint;
	Vector4 forward;
	Vector2 roadNetworkMax, roadNetworkMin;
	float rotationAngle, forwardDistance;
	std::vector<Segment> roadNetwork;
	std::vector<std::string> roadFiles;
	char forwardSymbol, turnLeftSymbol, turnRightSymbol, halfSymbol;

	bool segmentIntersection(Segment& first, Segment& second);
};

#endif // !ROADNET_H
