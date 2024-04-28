#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec2 TexCoords;
out mat3 TBN;
out vec4 FragPosLightSpace;


uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightProjection;
uniform mat4 lightView;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	vec3 T = normalize(vec3((view * model) * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3((view * model) * vec4(aNormal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	TBN = mat3(T, B, N);

	FragPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(view * model))) * aNormal; // fix non-uniform scaling
	LightPos = vec3(view * vec4(lightPos, 1.0));
	TexCoords = aTexCoords;


	vec3 FragPos2 = vec3(model * vec4(aPos, 1.0));
	// Shadow
	FragPosLightSpace = (lightProjection * lightView) * vec4(FragPos2, 1.0);
}
