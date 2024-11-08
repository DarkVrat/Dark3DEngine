#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 FragPos;  
out vec3 Normal;
out vec3 ViewPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
  
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
	ViewPos = vec3(inverse(view)[3]);
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	TexCoords = aTexCoords;
}