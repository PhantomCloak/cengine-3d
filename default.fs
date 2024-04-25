#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;

uniform vec3 viewPos;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
    float shininess;
}; 

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{
	// Ambient
	vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
	// specular
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Use the red channel as the specular intensity and apply it to the light's specular color.
	float specularIntensity = texture(material.texture_specular1, TexCoords).r;
	vec3 specular = light.specular * spec * specularIntensity;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}
