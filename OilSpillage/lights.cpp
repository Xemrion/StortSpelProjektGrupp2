#include "Lights.h"

LightList::LightList()
{

}

LightList::~LightList()
{

}

std::array<PointLight, LightList::maxSize>::iterator LightList::addLight(PointLight& light)
{
	auto returnIterator = firstEmptySpacePointLights;
	*firstEmptySpacePointLights = std::move(light);
	firstEmptySpacePointLights = findNextEmptySpace(firstEmptySpacePointLights);
	return returnIterator;
}

std::array<PointLight, LightList::maxSize>::iterator LightList::addLight(PointLight&& light)
{
	auto returnIterator = firstEmptySpacePointLights;
	*firstEmptySpacePointLights = std::move(light);
	firstEmptySpacePointLights = findNextEmptySpace(firstEmptySpacePointLights);
	return returnIterator;
}

std::array<SpotLight, LightList::maxSize>::iterator LightList::addLight(SpotLight& light)
{
	auto returnIterator = firstEmptySpaceSpotLights;
	*firstEmptySpaceSpotLights = std::move(light);
	firstEmptySpaceSpotLights = findNextEmptySpace(firstEmptySpaceSpotLights);
	return returnIterator;
}

std::array<SpotLight, LightList::maxSize>::iterator LightList::addLight(SpotLight&& light)
{
	auto returnIterator = firstEmptySpaceSpotLights;
	*firstEmptySpaceSpotLights = std::move(light);
	firstEmptySpaceSpotLights = findNextEmptySpace(firstEmptySpaceSpotLights);
	return returnIterator;
}

std::array<PointLight, LightList::maxSize>::iterator LightList::findNextEmptySpace(std::array<PointLight, maxSize>::iterator start)
{
	auto it = firstEmptySpacePointLights;
	for (; it != pointLights.end(); ++it)
	{
		if (it->getLuminance() == 0.0) 
		{
			break;
		}
	}

	return it;
}

std::array<SpotLight, LightList::maxSize>::iterator LightList::findNextEmptySpace(std::array<SpotLight, maxSize>::iterator start)
{
	auto it = firstEmptySpaceSpotLights;
	for (; it != spotLights.end(); ++it)
	{
		if (it->getLuminance() == 0.0)
		{
			break;
		}
	}

	return it;
}

void LightList::removeLight(std::array<PointLight, maxSize>::iterator lightIter)
{
	*lightIter = PointLight();
	if (lightIter < firstEmptySpacePointLights)
	{
		firstEmptySpacePointLights = lightIter;
	}
}

void LightList::removeLight(std::array<SpotLight, maxSize>::iterator lightIter)
{
	*lightIter = SpotLight();
	if (lightIter < firstEmptySpaceSpotLights)
	{
		firstEmptySpaceSpotLights = lightIter;
	}
}

void LightList::setSun(Sun sun)
{
	this->sun = sun;
}
