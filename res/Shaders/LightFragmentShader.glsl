#version 330 core
out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 colorLight;

void main()
{
    FragColor = vec4(colorLight, 1.0);
}