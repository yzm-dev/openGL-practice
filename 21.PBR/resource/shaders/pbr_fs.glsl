#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// PBR textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// camera
uniform vec3 camPos;

// lights
struct Light {
    vec3 position;
    vec3 color;
};
uniform Light lights[1];
uniform int lightCount;

// constants
const float PI = 3.14159265359;
const float EPSILON = 1e-5;

// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ---- helper functions ----
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, EPSILON);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k + EPSILON);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float a = roughness * roughness;
    float k = (a + 1.0) * (a + 1.0) / 8.0;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggxV = GeometrySchlickGGX(NdotV, k);
    float ggxL = GeometrySchlickGGX(NdotL, k);
    return ggxV * ggxL;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ---- main ----
void main()
{
    // textures (assume textures are linear and loaded as such)
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2)); // if albedo stored in sRGB, convert
    float metallic = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao = texture(aoMap, TexCoords).r;

    vec3 N = GetNormalFromMap();
    vec3 V = normalize(camPos - FragPos);
    vec3 R = reflect(-V, N);

    // compute F0
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // accumulate
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < lightCount; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lights[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * attenuation;

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            // Cook-Torrance BRDF
            float D = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 numerator = D * G * F;
            float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + EPSILON;
            vec3 specular = numerator / denom;

            // kS is Fresnel
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            // Lambertian diffuse
            vec3 diffuse = (albedo / PI);

            // final contribution
            Lo += (kD * diffuse + specular) * radiance * NdotL;
        }
    }

    // ambient (approx)
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemap (simple) + gamma correct
    color = color / (color + vec3(1.0)); // simple Reinhard
    color = pow(color, vec3(1.0/2.2)); // gamma

    FragColor = vec4(color, 1.0);
}
