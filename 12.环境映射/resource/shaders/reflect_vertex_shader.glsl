#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = vTexCoords;
    Normal = mat3(transpose(inverse(model))) * vNormal;
    Position = vec3(model * vec4(vPos, 1.0));
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}