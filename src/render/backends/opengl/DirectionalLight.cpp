#include "DirectonalLight.h"


void DirectionalLight::Draw(Shader& shader) {
	shader.setVec3("light.ambient", Ambient);
	shader.setVec3("light.diffuse", Diffuse);
	shader.setVec3("light.specular", Specular);
}
