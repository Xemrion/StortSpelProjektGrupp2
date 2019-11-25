#ifndef VEHICLE_STATS_H
#define VEHICLE_STATS_H

struct Stats
{
	int maxHealth = 100;
	float durability = 1.0F;
	float armour = 1.0F;
	float accelerationRate = 1.0F;
	float maxSpeed = 1.0F;
	float handlingRate = 1.0F;
};

class VehicleStats
{
public:
	static constexpr Stats defaultStats = Stats();

	                               //{ health, durability, armour, accelerationRate, maxSpeed, handlingRate };
	static constexpr Stats fastCar = {    100,       2.0F,   2.0F,             1.3F,     1.4F,         1.0F };

	//AI Stats
									//{ health, durability, armour, accelerationRate, maxSpeed, handlingRate };

	static constexpr Stats AIAttacker = { 50,       2.0f,    2.0f,          0.5,          3.0f,      1.0f };
									//{ health, durability, armour, accelerationRate, maxSpeed, handlingRate };
	static constexpr Stats AISniper   = { 100,       3.0f,    3.0f,          1.0,          7.0f,      2.0f };
									//{ health, durability, armour, accelerationRate, maxSpeed, handlingRate };
	static constexpr Stats AITurret   = { 75,       2.0f,    2.0f,          0.0,          0.0f,      1.0f };
									//{ health, durability, armour, accelerationRate, maxSpeed, handlingRate };
	static constexpr Stats AISwarm    = { 20,       1.0f,    1.0f,          2.0,          7.0f,      2.0f };

	static constexpr Stats AICar      = { 200,		2.0f,	2.0f,				1.0f,	0.7f,		10.0f };

	static constexpr Stats AIBoss	  = { 2000,		5.0f,	5.0f,				5.0f,	20.0f,		1.0f };
};

#endif // !VEHICLE_STATS_H
