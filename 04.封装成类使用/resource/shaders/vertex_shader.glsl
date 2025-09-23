#version 330 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main()
{
    // Apply the model transformation to the vertex position
    gl_Position = vec4(projection * view * model * vec4(vPos, 1.0));
    texCoord = aTexCoord;
};