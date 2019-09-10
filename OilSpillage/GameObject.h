#pragma once
#include "Mesh.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include <d3d11.h>
#include"Texture.h"
#include<SimpleMath.h>
using namespace DirectX::SimpleMath; 
class GameObject {
	Vector3 position;
	Vector3 scale=Vector3(1,1,1);
	Vector3 rotation;
	Texture* texture = nullptr;
public:
	const Mesh* mesh = nullptr;
	
	Matrix getTransform() {
		
		Quaternion qt = Quaternion::CreateFromYawPitchRoll(rotation.y,rotation.x,rotation.z);
		Matrix mtr = Matrix::CreateFromQuaternion(qt);
		Matrix translate = Matrix::CreateTranslation(position);
		Matrix scaleM = Matrix::CreateScale(scale);
		return   mtr * scaleM *translate;
			
	};

	void setPosition(Vector3 newPos)
	{
		this->position = newPos;
	};

	void move(Vector3 addPos)
	{
		this->position += addPos;
	};

	void addRotation(Vector3 addRotaiton)
	{
		this->rotation += addRotaiton;
	};

	void setRotation(Vector3 newRotation)
	{
		this->rotation = newRotation;
	};

	void setScale(Vector3 newScale)
	{
		this->scale = newScale;
	};
	
	Texture* getTexture()
	{
		return this->texture;
	};

	void setTexture(Texture* aTexture)
	{
		this->texture = aTexture;
	};
};
