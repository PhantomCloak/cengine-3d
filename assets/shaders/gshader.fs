#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
//in vec3 Normal;
in vec4 FragPosLightSpace;
//in mat3 TBN;
in vec3 LightPos;

in vec3 Normal0;
in vec3 WorldPos0;
in vec3 Tangent0;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_height1;
	float shininess;
}; 

uniform Material material;

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(Normal0);
    vec3 Tangent = normalize(Tangent0);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(material.texture_height1, TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main()
{    
	vec3 normal = CalcBumpedNormal();
	//vec3 normal = normalize(Normal);
	//vec3 normal = texture(material.texture_height1, TexCoords).rgb;
	//normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space


	// store the fragment position vector in the first gbuffer texture
	gPosition = vec4(FragPos, 1.0);
	// also store the per-fragment normals into the gbuffer
	gNormal = vec4(normal, 1.0);
	// and the diffuse per-fragment color
	gAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
	// store specular intensity in gAlbedoSpec's alpha component
	gAlbedoSpec.a = texture(material.texture_specular1, TexCoords).r;
} 
