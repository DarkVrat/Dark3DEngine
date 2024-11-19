#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform mat3 kernel;
uniform mat3 filter;
uniform float texelSize;
uniform float gamma;
uniform float exposure;

void main()
{
    vec3 col = vec3(0.0);
    for(int i = -1; i <= 1; ++i) {
        for(int j = -1; j <= 1; ++j) {
            vec2 offset = vec2(i*texelSize, j*texelSize);
            col += texture(screenTexture, TexCoords.st + offset).rgb * kernel[i+1][j+1];
        }
    }
    
	col = filter * col;
	
	vec3 mapped = vec3(1.0) - exp(-col * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	
    FragColor = vec4(mapped, 1.0);
}  