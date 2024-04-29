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
		float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
		 if(brightness > 1.0)
        FragColor = vec4(hdrColor, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
