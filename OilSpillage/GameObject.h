#pragma once
#include "Mesh.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

class GameObject {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
public:
	const Mesh* mesh = nullptr;
	glm::mat4 getTransform() {
		return glm::translate(glm::mat4(1.0), position);
	};
};
