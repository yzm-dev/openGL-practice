#version 330 core
out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    // ambient light
    vec3 ambient = light.ambient * material.ambient;

    // diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Blinn-Phong lighting model
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}