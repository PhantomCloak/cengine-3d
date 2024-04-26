#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_height1;
	float shininess;
}; 

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform bool test;

void main()
{
	float gamma = 2.2;
	float constant = 1.0;
	float linear = 0.002;
	float quadratic = 0.000027;

	float distance = length(LightPos - FragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 normal;
	normal = texture(material.texture_height1, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;   
	normal = normalize(TBN * normal); 	// Ambient

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * pow(texture(material.texture_diffuse1, TexCoords).rgb, vec3(gamma));
	//vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

	// specular
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Use the red channel as the specular intensity and apply it to the light's specular color.
	float specularIntensity = texture(material.texture_specular1, TexCoords).r;
	vec3 specular = light.specular * spec * specularIntensity;

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	//FragColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0);
	//FragColor = vec4(result, 1.0);
	FragColor = vec4(result, 1.0);
	//FragColor = texture(material.texture_diffuse1, TexCoords);
}

