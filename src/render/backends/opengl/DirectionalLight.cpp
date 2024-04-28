#include "DirectonalLight.h"
#include <glm/gtc/matrix_transform.hpp>


void DirectionalLight::Draw(Shader& shader) {
	shader.setVec3("lightPos", this->Transform.position);
	shader.setVec3("light.ambient", Ambient);
	shader.setVec3("light.diffuse", Diffuse);
	shader.setVec3("light.specular", Specular);
}

glm::mat4 DirectionalLight::GetViewMatrix() {
	float pitch = Transform.rotation.x;
	float yaw = Transform.rotation.y;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	// Normalize the front vector
	glm::vec3 Front = glm::normalize(front);
	glm::vec3 up = glm::vec3(0, 1, 0);

	return glm::lookAt(Transform.position, Transform.position + Front, up);
}
