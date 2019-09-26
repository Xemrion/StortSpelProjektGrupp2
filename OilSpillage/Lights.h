#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <array>
#include "ShaderDefines.hlsli"

using namespace DirectX::SimpleMath;

class PointLight
{
	friend class Graphics;
	Vector4 pos; // empty w component
	Vector4 color; // w component is luminance
	Vector4 padding = Vector4(0.0, 0.0, 0.0, 0.0);
public:
	PointLight() { pos = Vector4(); color = Vector4(); }
	PointLight(Vector3 pos, Vector3 color, float luminance) {
		this->pos = Vector4(pos);
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = Vector4(color);
		assert(luminance > 0.0f);
		this->color.w = luminance;
	}
	~PointLight() {}

	void setPos(Vector3 pos) { this->pos = Vector4(pos); }
	Vector3 getPos() const { return Vector3(pos); }
	void setColor(Vector3 color) {
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color.x = color.x;
		this->color.y = color.y;
		this->color.z = color.z;
	}
	Vector3 getColor() const { return Vector3(color); }
	void setLuminance(float luminance) { this->color.w = luminance; };
	float getLuminance() const { return this->color.w; }
};

class SpotLight
{
	friend class Graphics;
	Vector4 pos; // empty w component
	Vector4 color; // w component is luminance
	Vector4 directionWidth; //xyz direction, w width
public:
	SpotLight() { pos = Vector4(); color = Vector4(); directionWidth = Vector4(); }
	SpotLight(Vector3 pos, Vector3 color, float luminance, Vector3 direction, float conePercentOpen) {
		this->pos = Vector4(pos);
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = Vector4(color);
		this->color.w = luminance;
		direction.Normalize();
		this->directionWidth = Vector4(direction);
		conePercentOpen = min(max(0.0f, conePercentOpen), 1.0f);
		this->directionWidth.w = conePercentOpen * DirectX::XM_PIDIV2;
	}
	~SpotLight() {}

	void setPos(Vector3 pos) { this->pos = Vector4(pos); }
	Vector3 getPos() const { return Vector3(pos); }
	void setColor(Vector3 color) {
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color.x = color.x;
		this->color.y = color.y;
		this->color.z = color.z;
	}
	Vector3 getColor() const { return Vector3(color); }
	void setLuminance(float luminance) { this->color.w = luminance; };
	float getLuminance() const { return this->color.w; }
	void setDirection(Vector3 dir) { 
		this->directionWidth.x = dir.x;
		this->directionWidth.y = dir.y;
		this->directionWidth.z = dir.z;
	}
	Vector3 getDirection() const { return Vector3(directionWidth); }
	
	void setConeWidth(float percentOpen) { percentOpen = min(max(0.0f, percentOpen), 1.0f); directionWidth.w = DirectX::XM_PIDIV2 * percentOpen; }
};

class Sun {
	Vector4 color;
	Vector4 direction;
public:
	Sun() : direction(0.0, -1.0, 0.0, 0.0), color(1.0, 1.0, 1.0, 1.0) {}
	Sun(Vector3 dir, Vector3 color) {
		dir.Normalize();
		color.Clamp(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 1.0, 1.0));
		this->direction = Vector4(dir.x, dir.y, dir.z, 0.0);
		this->color = Vector4(color.x, color.y, color.z, 1.0);
	}
	
	void setDirection(Vector3& dir) {
		dir.Normalize();
		this->direction = Vector4(dir);
	}
	Vector3 getDirection() const { return Vector3(direction); }
	void setColor(Vector3 color) {
		color.Clamp(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 1.0, 1.0));
		this->color = Vector4(color);
	}
	Vector3 getColor() const { return Vector3(this->color); }
};

class LightList {
public:
	static const size_t maxSize = MAX_LIGHTS_TOTAL;

	LightList();
	~LightList();

	std::array<PointLight, maxSize>::iterator addLight(PointLight& light);
	std::array<PointLight, maxSize>::iterator addLight(PointLight&& light);
	std::array<SpotLight, maxSize>::iterator addLight(SpotLight& light);
	std::array<SpotLight, maxSize>::iterator addLight(SpotLight&& light);
	void removeLight(std::array<PointLight, maxSize>::iterator);
	void removeLight(std::array<SpotLight, maxSize>::iterator);
	void setSun(Sun sun);
private:
	friend class Graphics;
	std::array<PointLight, maxSize> pointLights;
	std::array<PointLight, maxSize>::iterator firstEmptySpacePointLights = pointLights.begin();
	std::array<SpotLight, maxSize> spotLights;
	std::array<SpotLight, maxSize>::iterator firstEmptySpaceSpotLights = spotLights.begin();

	std::array<PointLight, maxSize>::iterator findNextEmptySpace(std::array<PointLight, maxSize>::iterator start);
	std::array<SpotLight, maxSize>::iterator findNextEmptySpace(std::array<SpotLight, maxSize>::iterator start);

	Sun sun;
};
