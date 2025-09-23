#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 Normal;
} gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 projection;

void GenerateLine(int index)
{
    vec3 normal = gs_in[index].Normal;
    vec4 start = gl_in[index].gl_Position;
    vec4 end = start + vec4(normal * MAGNITUDE, 0.0);
    gl_Position = projection * start;
    EmitVertex();
    gl_Position = projection * end;
    EmitVertex();
    EndPrimitive();
}

void main() 
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}