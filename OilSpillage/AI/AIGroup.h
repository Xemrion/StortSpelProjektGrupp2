#pragma once
#include"DynamicActor.h"
class AIGroup
{
public:
	AIGroup();
	~AIGroup();
	void update(const Vector3& targetPos);
	const Vector3& getAveragePos() const;
	void updateAveragePos();
	void setPath(std::vector<Vector3> path);
	void updateDuty();
	const float getGroupRadius() const;
private:
	void removeDeadActors();
	float groupRadius; //Compare with distance^2
	friend class ActorManager;
	std::vector<DynamicActor*> actors;
	Vector3 averagePos;
	std::vector<Vector3> path;
};

