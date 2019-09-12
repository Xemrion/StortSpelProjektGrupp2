#pragma once
#include "Mesh.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include <d3d11.h>
#include"Texture.h"
#include<math.h>
using namespace DirectX::SimpleMath; 
class GameObject {
	Vector3 position;
	Vector3 scale=Vector3(1,1,1);
	Vector3 rotation;
	Vector4 color;
	Texture* texture = nullptr;
public:
	const Mesh* mesh = nullptr;
	
	Matrix getTransform();

	void setPosition(Vector3 newPos);
	void move(Vector3 addPos);
	void addRotation(Vector3 addRotaiton);
	void setRotation(Vector3 newRotation);
	void setScale(Vector3 newScale);
	Texture* getTexture();
	void setTexture(Texture* aTexture);
	void setColor(Vector4 aColor);
	Vector4 getColor()const;
	
};
