#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.texCoords = vTexCoords;    
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}