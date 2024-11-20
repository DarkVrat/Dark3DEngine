#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform bool horizontal;
uniform sampler2D image;

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);  // texel size
    vec3 result = vec3(0.0);
    float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    for (int i = -4; i < 5; ++i) {
        vec2 offset = horizontal ? vec2(tex_offset.x * i, 0.0) : vec2(0.0, tex_offset.y * i);
        result += texture(image, TexCoords + offset).rgb * weight[abs(i)];
    }
    FragColor = vec4(result, 1.0);
}