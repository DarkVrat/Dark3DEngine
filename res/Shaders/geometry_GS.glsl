#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

out vec3 fColor;

const float MAGNITUDE = 0.03;
  
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
	fColor = vec3(1.0, 1.0, 0.0);
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
	fColor = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  