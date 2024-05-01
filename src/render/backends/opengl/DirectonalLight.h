#pragma once
#include "render/backends/opengl/Shader.h"
#include "scene/node.h"
#include <glm/glm.hpp>


class DirectionalLight : public Node {
	public:
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		float FarPlane;
		float NearPlane;

		DirectionalLight() : Ambient(glm::vec3(1)), Diffuse(glm::vec3(1)), Specular(glm::vec3(1)), FarPlane(4000), NearPlane(0.1f) {
			Name = "Light";
		};

		DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float farPlane, float nearPlane) : Ambient(ambient), Diffuse(glm::vec3(diffuse)), Specular(specular), FarPlane(farPlane), NearPlane(nearPlane) {
			Name = "Light";
		};

		void Draw(Shader &shader);
		
		glm::mat4 GetViewMatrix();
};
