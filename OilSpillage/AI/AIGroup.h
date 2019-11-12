#pragma once
#include"DynamicActor.h"
class AIGroup
{
public:
	AIGroup();
	~AIGroup();

	const Vector3& getAveragePos() const;
	void updateAveragePos();
	std::vector<Vector3>* getPathPtr();
	void setPath(std::vector<Vector3> path);
	void formationP(const Vector3& targetPos);
	void updateDuty();
	const float getGroupRadius() const;
private:
	float groupRadius; //Compare with distance^2
	friend class ActorManager;
	std::vector<DynamicActor*> actors;
	Vector3 averagePos;
	std::vector<Vector3> path;
};

