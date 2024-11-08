#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec3 cameraPos;

uniform samplerCube skybox;
uniform float ratio;

void main()
{             
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
} 