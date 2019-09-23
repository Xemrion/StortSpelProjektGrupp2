#include "Lights.h"

LightList::LightList()
{

}

LightList::~LightList()
{

}

std::array<PointLight, LightList::size>::iterator LightList::addLight(PointLight& light)
{
	auto returnIterator = firstEmptySpacePointLights;
	*firstEmptySpacePointLights = std::move(light);
	firstEmptySpacePointLights = findNextEmptySpace(firstEmptySpacePointLights);
	return returnIterator;
}

std::array<PointLight, LightList::size>::iterator LightList::addLight(PointLight&& light)
{
	auto returnIterator = firstEmptySpacePointLights;
	*firstEmptySpacePointLights = std::move(light);
	firstEmptySpacePointLights = findNextEmptySpace(firstEmptySpacePointLights);
	return returnIterator;
}

std::array<SpotLight, LightList::size>::iterator LightList::addLight(SpotLight& light)
{
	auto returnIterator = firstEmptySpaceSpotLights;
	*firstEmptySpaceSpotLights = std::move(light);
	firstEmptySpaceSpotLights = findNextEmptySpace(firstEmptySpaceSpotLights);
	return returnIterator;
}

std::array<SpotLight, LightList::size>::iterator LightList::addLight(SpotLight&& light)
{
	auto returnIterator = firstEmptySpaceSpotLights;
	*firstEmptySpaceSpotLights = std::move(light);
	firstEmptySpaceSpotLights = findNextEmptySpace(firstEmptySpaceSpotLights);
	return returnIterator;
}

std::array<PointLight, LightList::size>::iterator LightList::findNextEmptySpace(std::array<PointLight, size>::iterator start)
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

std::array<SpotLight, LightList::size>::iterator LightList::findNextEmptySpace(std::array<SpotLight, size>::iterator start)
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

void LightList::removeLight(std::array<PointLight, size>::iterator lightIter)
{
	*lightIter = PointLight();
	if (lightIter < firstEmptySpacePointLights)
	{
		firstEmptySpacePointLights = lightIter;
	}
}

void LightList::removeLight(std::array<SpotLight, size>::iterator lightIter)
{
	*lightIter = SpotLight();
	if (lightIter < firstEmptySpaceSpotLights)
	{
		firstEmptySpaceSpotLights = lightIter;
	}
}
