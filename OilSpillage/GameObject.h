#pragma once
#include "Mesh.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

class GameObject {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec2 rotation;
public:
	const Mesh* mesh = nullptr;
	glm::mat4 getTransform() {
		
		return glm::translate(glm::mat4(1.0), position);
	};

	void setPosition(glm::vec3 newPos)
	{
		this->position = newPos;
	};

	void move(glm::vec3 addPos)
	{
		this->position += addPos;
	};
};
