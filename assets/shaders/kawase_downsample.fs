#version 330 core

uniform sampler2D u_sourceTex;
uniform vec2 u_sourceRes;

in vec2 TexCoords;

out vec3 FragColor;

// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
// This shader performs downsampling on a texture, as taken from Call Of Duty method, presented at ACM Siggraph 2014.
// This particular method was customly designed to eliminate "pulsating artifacts and temporal stability issues".
void main() {
    vec2 texelSize = 1.0f / u_sourceRes;
    float x = texelSize.x;
    float y = texelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ("e" is the current texel) ===
    vec3 a = texture(u_sourceTex, vec2(TexCoords.x - 2.0f * x, TexCoords.y + 2.0f * y)).rgb;
    vec3 b = texture(u_sourceTex, vec2(TexCoords.x, TexCoords.y + 2.0f * y)).rgb;
    vec3 c = texture(u_sourceTex, vec2(TexCoords.x + 2.0f * x, TexCoords.y + 2.0f * y)).rgb;

    vec3 d = texture(u_sourceTex, vec2(TexCoords.x - 2.0f * x, TexCoords.y)).rgb;
    vec3 e = texture(u_sourceTex, vec2(TexCoords.x, TexCoords.y)).rgb;
    vec3 f = texture(u_sourceTex, vec2(TexCoords.x + 2.0f * x, TexCoords.y)).rgb;

    vec3 g = texture(u_sourceTex, vec2(TexCoords.x - 2.0f * x, TexCoords.y - 2.0f * y)).rgb;
    vec3 h = texture(u_sourceTex, vec2(TexCoords.x, TexCoords.y - 2.0f * y)).rgb;
    vec3 i = texture(u_sourceTex, vec2(TexCoords.x + 2.0f * x, TexCoords.y - 2.0f * y)).rgb;

    vec3 j = texture(u_sourceTex, vec2(TexCoords.x - x, TexCoords.y + y)).rgb;
    vec3 k = texture(u_sourceTex, vec2(TexCoords.x + x, TexCoords.y + y)).rgb;
    vec3 l = texture(u_sourceTex, vec2(TexCoords.x - x, TexCoords.y - y)).rgb;
    vec3 m = texture(u_sourceTex, vec2(TexCoords.x + x, TexCoords.y - y)).rgb;

    // Apply weighted distribution: 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap, so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.) contribute 0.5 to the final color output. The code below is written to effectively yield this sum.
    // We get: 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    FragColor = e * 0.125f;
    FragColor += (a + c + g + i) * 0.03125f;
    FragColor += (b + d + f + h) * 0.0625f;
    FragColor += (j + k + l + m) * 0.125f;
}
