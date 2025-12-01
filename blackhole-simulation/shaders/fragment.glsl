#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform vec3 viewPos;

void main() {
    // Distance from fragment to black hole center
    float dist = length(FragPos - blackHolePos);
    
    // Calculate event horizon effect
    float horizonGlow = 1.0 - smoothstep(schwarzschildRadius * 0.8, schwarzschildRadius * 1.2, dist);
    
    // Lighting calculation
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient lighting
    vec3 ambient = vec3(0.1, 0.05, 0.05);
    
    // Create a rim lighting effect for the event horizon
    float rimIntensity = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);
    vec3 rimColor = vec3(1.0, 0.5, 0.0) * rimIntensity * horizonGlow;
    
    // Black hole core (completely dark)
    vec3 coreColor = vec3(0.0, 0.0, 0.0);
    
    // Mix between core and rim
    vec3 finalColor = mix(coreColor, rimColor + ambient, horizonGlow);
    
    FragColor = vec4(finalColor, 1.0);
}