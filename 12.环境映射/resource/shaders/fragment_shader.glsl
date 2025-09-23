#version 330 core
out vec4 fragColor;

in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox; //slot = 2

uniform sampler2D texture_diffuse1; //slot = 0
uniform sampler2D texture_reflect1; //slot = 1

in vec3 Normal;
in vec3 FragPos;

void main()
{    
    vec4 diffuseColor = texture(texture_diffuse1, TexCoords);

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-viewDir, normalize(Normal));
    vec4 reflectColor = texture(skybox, reflectDir);

    float reflectFactor = texture(texture_reflect1, TexCoords).r;
    fragColor = mix(diffuseColor, reflectColor, reflectFactor);
}