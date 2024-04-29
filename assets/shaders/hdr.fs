#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;
uniform bool gamma_correction;
uniform bool use_reinhard;
uniform float exposure;

void main()
{             
	const float gamma = 2.2;
	vec3 hdrColor = texture(image, TexCoords).rgb;

	if(gamma_correction)
	{
		// reinhard tone mapping
		vec3 mapped;
		if(use_reinhard)
		{
			mapped = hdrColor / (hdrColor + vec3(1.0));
		}
		else
		{
			mapped = vec3(1.0) - exp(-hdrColor * exposure); // exposure
		}
		// gamma correction 
		mapped = pow(mapped, vec3(1.0 / gamma));

		FragColor = vec4(mapped, 1.0);
	}
	else
	{
		FragColor = vec4(hdrColor, 1.0);
	}
}
