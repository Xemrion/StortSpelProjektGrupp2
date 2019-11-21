#include "Lights.h"

LightList::LightList()
{
	firstEmptySpace = 0;
}

LightList::~LightList()
{

}

PointLight* LightList::addLight(PointLight& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (PointLight*)&lights[returnIterator];
}

PointLight* LightList::addLight(PointLight&& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (PointLight*)&lights[returnIterator];
}

SpotLight* LightList::addLight(SpotLight& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (SpotLight*)&lights[returnIterator];
}

SpotLight* LightList::addLight(SpotLight&& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (SpotLight*)&lights[returnIterator];
}

LaserLight* LightList::addLight(LaserLight& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (LaserLight*)& lights[returnIterator];
}

LaserLight* LightList::addLight(LaserLight&& light)
{
	auto returnIterator = firstEmptySpace;
	lights[firstEmptySpace] = std::move(light);
	firstEmptySpace = findNextEmptySpace(firstEmptySpace);
	return (LaserLight*)& lights[returnIterator];
}

UINT LightList::findNextEmptySpace(UINT start)
{
	UINT nextEmptySpace = start;
	for (; nextEmptySpace < maxSize; ++nextEmptySpace)
	{
		if (lights[nextEmptySpace].getLuminance() == 0.0)
		{
			break;
		}
	}

	return nextEmptySpace;
}

void LightList::removeLight(PointLight* lightPtr)
{
	*lightPtr = PointLight();
	if (lightPtr < (lights.data() + sizeof(Light) * firstEmptySpace))
	{
		Light* lightsStartPtr = lights.data();
		firstEmptySpace = ((UINT)lightPtr - (UINT)lightsStartPtr) / sizeof(Light);
	}
}

void LightList::removeLight(SpotLight* lightPtr)
{
	*lightPtr = SpotLight();
	if (lightPtr < (lights.data() + sizeof(Light) * firstEmptySpace))
	{
		Light* lightsStartPtr = lights.data();
		firstEmptySpace = ((UINT)lightPtr - (UINT)lightsStartPtr) / sizeof(Light);
	}
}

void LightList::removeLight(LaserLight* lightPtr)
{
	*lightPtr = LaserLight();
	if (lightPtr < (lights.data() + sizeof(Light) * firstEmptySpace))
	{
		Light* lightsStartPtr = lights.data();
		firstEmptySpace = ((UINT)lightPtr - (UINT)lightsStartPtr) / sizeof(Light);
	}
}

void LightList::setSun(Sun sun)
{
	this->sun = sun;
}

Sun LightList::getSun()
{
	return this->sun;
}
