#version 330 core

out vec4 FragColor;
out vec4 DepthColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
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


uniform sampler2D shadowMap;

float near = 0.1; 
float far  = 4000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//float bias = 0.005;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	const int halfkernelWidth = 3;
	for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
	{
		for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));	if(projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;
}

void main()
{
	float gamma = 2.2;
	float distance = length(LightPos - FragPos);
	vec4 textureColor = pow(texture(material.texture_diffuse1, TexCoords), vec4(gamma));
	//pow(texture(diffuse, texCoords).rgb, vec3(gamma));

	// Hack
	if(textureColor.a < 0.5) {
		discard;
	}

	vec3 normal;
	normal = texture(material.texture_height1, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;   
	normal = normalize(TBN * normal); 	// Ambient

	vec3 ambient = light.ambient * vec3(1);

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff;

	// specular
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * light.specular;

	float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
	//float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
	vec3 resultLight = (ambient + (1.0 - shadow) * (diffuse + specular)) * textureColor.rgb;


	FragColor = vec4(resultLight, 1.0);

	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    DepthColor = vec4(vec3(depth), 1.0);
}

