#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 viewPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	viewPos = vec3(inverse(view)[3]);
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}