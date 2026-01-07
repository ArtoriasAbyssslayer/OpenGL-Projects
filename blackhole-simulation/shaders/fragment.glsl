#version 430 core
out vec4 FragColor;
out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform vec3 viewPos;

// PBR Material properties
const vec3 albedo = vec3(0.05, 0.05, 0.1);
const float metallic = 0.9;
const float roughness = 0.3;
const float ao = 1.0;

const float PI = 3.14159265359;

// Distribution function
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry function
float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel function
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // Distance from fragment to black hole center
    float dist = length(FragPos - blackHolePos);
    
    // Event horizon - completely black inside
    if (dist < schwarzschildRadius) {
        discard;
    }
    
    // Photon sphere effect (1.5x event horizon)
    float photonSphere = schwarzschildRadius * 1.5;
    
    // Calculate gravitational lensing distortion
    vec3 toBlackHole = normalize(blackHolePos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    float lensingStrength = schwarzschildRadius / (dist * dist);
    lensingStrength = clamp(lensingStrength, 0.0, 1.0);
    
    // Apply lensing distortion to view direction
    vec3 distortedViewDir = normalize(viewDir + toBlackHole * lensingStrength * 0.5);
    
    // Calculate gravitational redshift effect
    float redshiftFactor = 1.0 - (schwarzschildRadius / dist);
    redshiftFactor = clamp(redshiftFactor, 0.0, 1.0);
    
    // Accretion disk glow effect
    float accretionGlow = 0.0;
    if (dist < photonSphere * 2.0) {
        accretionGlow = 1.0 - smoothstep(photonSphere, photonSphere * 2.0, dist);
    }
    
    // Lighting calculation with PBR
    vec3 norm = normalize(Normal);
    vec3 N = norm;
    vec3 V = distortedViewDir;
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // Light sources (hot accretion disk)
    vec3 lightPositions[3];
    vec3 lightColors[3];
    
    // Multiple light sources around black hole
    float angle1 = 0.0, angle2 = 2.094, angle3 = 4.189;
    float lightDist = schwarzschildRadius * 3.0;
    
    lightPositions[0] = blackHolePos + vec3(cos(angle1), 0.5, sin(angle1)) * lightDist;
    lightPositions[1] = blackHolePos + vec3(cos(angle2), 0.5, sin(angle2)) * lightDist;
    lightPositions[2] = blackHolePos + vec3(cos(angle3), 0.5, sin(angle3)) * lightDist;
    
    lightColors[0] = vec3(300.0, 250.0, 200.0) * accretionGlow; // Hot plasma
    lightColors[1] = vec3(350.0, 300.0, 250.0) * accretionGlow;
    lightColors[2] = vec3(400.0, 350.0, 300.0) * accretionGlow;
    
    // Reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 3; ++i) {
        vec3 L = normalize(lightPositions[i] - FragPos);
        vec3 H = normalize(V + L);
        
        float distance = length(lightPositions[i] - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    // Einstein ring effect - enhanced brightness at specific angles
    float einsteinRing = 0.0;
    float ringRadius = schwarzschildRadius * 2.6;
    if (abs(dist - ringRadius) < schwarzschildRadius * 0.3) {
        einsteinRing = 1.0 - abs(dist - ringRadius) / (schwarzschildRadius * 0.3);
    }
    
    // Einstein ring emission
    vec3 einsteinEmission = vec3(0.9, 0.95, 1.0) * einsteinRing * 50.0;
    
    // Ambient lighting
    vec3 ambient = vec3(0.01, 0.005, 0.02) * albedo * ao;
    
    vec3 color = ambient + Lo + einsteinEmission;
    
    // Apply gravitational redshift and lensing
    color = color * redshiftFactor;
    color = mix(color, vec3(0.8, 0.9, 1.0), lensingStrength * 0.3);
    
    // HDR tone mapping
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
    
    // Bright color threshold for bloom
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}