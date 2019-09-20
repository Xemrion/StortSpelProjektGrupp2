#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class PointLight
{
	Vector4 pos; // empty w component
	Vector4 color; // w component is luminance
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
		conePercentOpen = min(max(0.0, conePercentOpen), 1.0);
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
	
	void setConeWidth(float percentOpen) { percentOpen = min(max(0.0, percentOpen), 1.0); directionWidth.w = DirectX::XM_PIDIV2 * percentOpen; }
};