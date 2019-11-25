#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <array>
#include "ShaderDefines.hlsli"

using namespace DirectX::SimpleMath;

class Light {
	friend class Graphics;
protected:
	Vector3 pos;
	float lightType = -1;
	Vector3 color;
	float luminance;
	Vector3 direction;
	float coneWidth;
public:
	Light() : pos(0.0, 0.0, 0.0), color(0.0, 0.0, 0.0), luminance(0.0), direction(0.0, 0.0, 0.0), coneWidth(0.0) {};
	void setPos(Vector3 pos) { this->pos = Vector3(pos); }
	Vector3 getPos() const { return Vector3(pos); }
	void setColor(Vector3 color) {
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = color;
	}
	Vector3 getColor() const { return Vector3(color); }
	void setLuminance(float luminance) { this->luminance = luminance; };
	float getLuminance() const { return this->luminance; }
	float getLightType() const { return this->lightType; }
};

class PointLight : public Light
{
	friend class Graphics;
public:
	PointLight() {};
	PointLight(Vector3 pos, Vector3 color, float luminance) {
		this->pos = pos;
		this->lightType = 0;
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = color;
		this->luminance = luminance;
	}
	~PointLight() {}
};

class SpotLight : public Light
{
	friend class Graphics;
public:
	SpotLight() {};
	SpotLight(Vector3 pos, Vector3 color, float luminance, Vector3 direction, float conePercentOpen) {
		this->pos = pos;
		this->lightType = 1;
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = color;
		this->luminance = luminance;
		direction.Normalize();
		this->direction = direction;
		this->coneWidth = min(max(0.0f, conePercentOpen), 1.0f) * DirectX::XM_PIDIV2;
	}
	~SpotLight() {}

	void setDirection(Vector3 dir) {
		this->direction = dir;
	}
	Vector3 getDirection() const { return direction; }
	void setConeWidth(float percentOpen) { coneWidth = min(max(0.0f, percentOpen), 1.0f) * DirectX::XM_PIDIV2; }
};

class LaserLight : public Light
{
	friend class Graphics;
public:
	LaserLight() {};
	LaserLight(Vector3 pos, Vector3 color, float luminance, Vector3 direction, float length) {
		this->pos = pos;
		this->lightType = 2;
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color = color;
		this->luminance = luminance;
		direction.Normalize();
		this->direction = direction;
		this->coneWidth = length;
	}
	~LaserLight() {}

	void setDirection(Vector3 dir) {
		this->direction = dir;
	}
	void setLength(float length) {
		this->coneWidth = length;
	}
	Vector3 getDirection() const { return direction; }
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

	PointLight* addLight(PointLight& light);
	PointLight* addLight(PointLight&& light);
	SpotLight* addLight(SpotLight& light);
	SpotLight* addLight(SpotLight&& light);
	LaserLight* addLight(LaserLight& light);
	LaserLight* addLight(LaserLight&& light);
	void removeLight(PointLight* light);
	void removeLight(SpotLight* light);
	void removeLight(LaserLight* light);
	void setSun(Sun sun);
	Sun getSun();
private:
	friend class Graphics;
	std::array<Light, maxSize> lights;
	UINT firstEmptySpace;
	UINT findNextEmptySpace(UINT start);

	Sun sun;
};
