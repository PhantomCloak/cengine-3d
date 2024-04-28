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
    float roll = Transform.rotation.z; // Assuming roll is stored in z

    // Calculate the front vector from yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normalize the front vector
    glm::vec3 Front = glm::normalize(front);

    // Default world up vector
    glm::vec3 worldUp = glm::vec3(0, 1, 0);

    // Calculate the right vector
    glm::vec3 right = glm::normalize(glm::cross(Front, worldUp));

    // Calculate the adjusted up vector by rotating the world up vector around the right vector by the roll angle
    glm::vec3 up = glm::rotate(glm::mat4(1.0f), glm::radians(roll), right) * glm::vec4(worldUp, 1.0);

    // Return the view matrix using the updated up vector
    return glm::lookAt(Transform.position, Transform.position + Front, up);
}
