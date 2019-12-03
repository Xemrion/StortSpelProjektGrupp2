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
	std::vector<Vector3>* getPathPtr();
	void setPath(std::vector<Vector3> path);
	void updateDuty();
	const float getGroupRadius() const;
private:
	void removeDeadActors();
	float groupRadius; //Compare with distance^2
	float defaultGroupRadius = 6.5f * 6.5f; //Compare with distance^2
	float expandedGroupRadius = 6.5f * 6.5f * 2; //Compare with distance^2
	friend class ActorManager;
	std::vector<DynamicActor*> actors;
	Vector3 averagePos;
	std::vector<Vector3> path;
};

