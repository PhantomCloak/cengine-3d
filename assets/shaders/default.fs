#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 DepthColor;
layout(location = 2) out vec4 BrightColor;

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
uniform vec3 brightCutoff;

float near = 0.1; 
float far  = 4000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform pespective  divide (e.g get fragment location in light space)
	// By the way since we are using orthographic projection this step is meaningless due to w is untouched
	// But required if we are about to use perspective projection for the light
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Gets the result from -1 to 1
	//vec3 projCoords = fragPosLightSpace.xyz;

	// since depth texture is in 0 to 1 space we convert
	projCoords = projCoords * 0.5 + 0.5;

	// Since we mapped 1:1 with our depth map and we can simply get depth from r value of the depth texture
	float closestDepth = texture(shadowMap, projCoords.xy).r; 

	// get the fargment depth from Z component
	float currentDepth = projCoords.z;

	// Depending on the angle between fragment normal and light direction we increase our decrease bias
	float bias = max(0.0020 * (1.0 - dot(normal, lightDir)), 0.00020);

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
	shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));	

	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main()
{
	vec4 textureColor = texture(material.texture_diffuse1, TexCoords);

	// HACK: filter out semi transparent stuff
	if(textureColor.a < 0.5) {
		discard;
	}

	vec3 normal;
	normal = texture(material.texture_height1, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;   
	normal = normalize(TBN * normal);

	// Ambient
	vec3 ambient = light.ambient;

	// Diffuse
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(normal, lightDir), 0.0); // prevent diffuse if light below the normal
	vec3 diffuse = light.diffuse * diff;


	// specular
	vec3 viewDir = normalize(-FragPos);

	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * light.specular;

	float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
	vec3 resultLight = (ambient + (1.0 - shadow) * (diffuse + specular)) * textureColor.rgb;

	// Fragment
	FragColor = vec4(resultLight, 1.0);

	// Depth
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	DepthColor = vec4(vec3(depth), 1.0);

	// Bright Color
	//float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//vec3 resultLight = (ambient + (1.0 - shadow) * (diffuse + specular)) * textureColor.rgb;
	float brightness = dot(FragColor.rgb, brightCutoff);

	if(brightness > 1.0)
        BrightColor = FragColor;
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

