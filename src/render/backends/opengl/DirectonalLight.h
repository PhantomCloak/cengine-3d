#pragma once
#include "render/backends/opengl/Shader.h"
#include "scene/node.h"
#include <glm/glm.hpp>


class DirectionalLight : public Node {
	public:
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		DirectionalLight() : Ambient(glm::vec3(1)), Diffuse(glm::vec3(1)), Specular(glm::vec3(1)) {
			Name = "Light";
		};

		DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Ambient(ambient), Diffuse(glm::vec3(diffuse)), Specular(specular) {
			Name = "Light";
		};

		void Draw(Shader &shader);
		
		glm::mat4 GetViewMatrix();
};
