#version 330 core

out vec4 FragColor;
out vec4 DepthColor;

in vec2 TexCoords;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_height1;
	float shininess;
}; 

uniform Material material;

void main()
{
	FragColor = texture(material.texture_diffuse1, TexCoords);
}
