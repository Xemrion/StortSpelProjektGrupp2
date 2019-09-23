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
		this->color = Vector4(color);
		this->color.w = luminance;
	}
	~PointLight() {}

	void setPos(Vector3 pos) { this->pos = Vector4(pos); }
	Vector3 getPos() { return Vector3(pos); }
	void setColor(Vector3 color) {
		color.Clamp(Vector3(0.0), Vector3(1.0));
		this->color.x = color.x;
		this->color.y = color.y;
		this->color.z = color.z;
	}
	Vector3 getColor() { return Vector3(color); }
	void setLuminance(float luminance) { this->color.w = luminance; };
	float getLuminance() { return this->color.w; }
};