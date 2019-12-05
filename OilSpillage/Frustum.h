#pragma once
#include <SimpleMath.h>

struct Frustum {
	Vector4 nearPlane;
	Vector4 farPlane;
	Vector4 leftPlane;
	Vector4 rightPlane;
	Vector4 topPlane;
	Vector4 bottomPlane;

	inline static float pointToPlaneDistance(Vector4 plane, Vector3 point)
	{
		//return XMVector3Dot(plane, point);
		return (plane.x * point.x + plane.y * point.y + plane.z * point.z) + plane.w;//point.Dot(Vector3(plane)) + plane.w;
	}

	bool intersect(AABB box, float bias = 0.0, bool partialFrustum = true)
	{
		Vector3 p = box.minPos;
		if (leftPlane.x < 0) p.x = box.maxPos.x;
		if (leftPlane.y < 0) p.y = box.maxPos.y;
		if (leftPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(leftPlane, p) > bias) return false;

		p = box.minPos;
		if (rightPlane.x < 0) p.x = box.maxPos.x;
		if (rightPlane.y < 0) p.y = box.maxPos.y;
		if (rightPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(rightPlane, p) > bias) return false;

		p = box.minPos;
		if (topPlane.x < 0) p.x = box.maxPos.x;
		if (topPlane.y < 0) p.y = box.maxPos.y;
		if (topPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(topPlane, p) > bias) return false;

		p = box.minPos;
		if (bottomPlane.x < 0) p.x = box.maxPos.x;
		if (bottomPlane.y < 0) p.y = box.maxPos.y;
		if (bottomPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(bottomPlane, p) > bias) return false;

		if (partialFrustum) return true;

		p = box.minPos;
		if (nearPlane.x < 0) p.x = box.maxPos.x;
		if (nearPlane.y < 0) p.y = box.maxPos.y;
		if (nearPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(nearPlane, p) > bias) return false;

		p = box.minPos;
		if (farPlane.x < 0) p.x = box.maxPos.x;
		if (farPlane.y < 0) p.y = box.maxPos.y;
		if (farPlane.z < 0) p.z = box.maxPos.z;
		if (pointToPlaneDistance(farPlane, p) > bias) return false;

		return true;
	}
};