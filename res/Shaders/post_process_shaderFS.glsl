#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform mat3 kernel;
uniform mat3 filter;
uniform float texelSize;

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
	
    FragColor = vec4(col, 1.0);
}  