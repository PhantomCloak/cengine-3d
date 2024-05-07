#version 330 core

precision mediump float;

uniform sampler2D u_sourceTex;

in vec2 v_texCoord;

out vec3 FragColor;

// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
// This shader performs upsampling on a texture, as taken from Call Of Duty method, presented at ACM Siggraph 2014.
void main() {
    // The filter kernel is applied with a radius, specified in texture coordinates, so that the radius will vary across mip resolutions.
		float u_radius = 0.005;
    float x = u_radius;
    float y = u_radius;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ("e" is the current texel) ===
    vec3 a = texture(u_sourceTex, vec2(v_texCoord.x - x, v_texCoord.y + y)).rgb;
    vec3 b = texture(u_sourceTex, vec2(v_texCoord.x, v_texCoord.y + y)).rgb;
    vec3 c = texture(u_sourceTex, vec2(v_texCoord.x + x, v_texCoord.y + y)).rgb;

    vec3 d = texture(u_sourceTex, vec2(v_texCoord.x - x, v_texCoord.y)).rgb;
    vec3 e = texture(u_sourceTex, vec2(v_texCoord.x, v_texCoord.y)).rgb;
    vec3 f = texture(u_sourceTex, vec2(v_texCoord.x + x, v_texCoord.y)).rgb;

    vec3 g = texture(u_sourceTex, vec2(v_texCoord.x - x, v_texCoord.y - y)).rgb;
    vec3 h = texture(u_sourceTex, vec2(v_texCoord.x, v_texCoord.y - y)).rgb;
    vec3 i = texture(u_sourceTex, vec2(v_texCoord.x + x, v_texCoord.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    FragColor = e * 4.0f;
    FragColor += (b + d + f + h) * 2.0f;
    FragColor += (a + c + g + i);
    FragColor *= 1.0f / 16.0f;
}
