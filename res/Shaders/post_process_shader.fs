#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

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
	
	vec3 hdrColor = vec3(1.0) - exp(-col * exposure);
    hdrColor = pow(hdrColor, vec3(1.0 / gamma));

    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;

    vec3 finalColor = hdrColor + bloomColor;
    finalColor = clamp(finalColor, 0.0, 1.0);

    FragColor = vec4(finalColor, 1.0);
}  