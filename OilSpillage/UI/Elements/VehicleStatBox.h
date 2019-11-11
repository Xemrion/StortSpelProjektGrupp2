#ifndef VEHICLE_STAT_BOX_H
#define VEHICLE_STAT_BOX_H

#include "Element.h"
#include "../../Texture.h"
#include "../../VehicleStats.h"

class VehicleStatBox : public Element
{
private:
	Texture* textureFG;
	Texture* textureBG;
	float amounts[6];
public:
	static Vector2 size;

	VehicleStatBox(Vector2 position = Vector2());
	virtual ~VehicleStatBox();

	void draw(bool selected);
	void update(const Stats& stats);
};

#endif // !VEHICLE_STAT_BOX_H
