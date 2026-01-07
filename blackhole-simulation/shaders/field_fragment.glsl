#version 430 core
out vec4 FragColor;
out vec4 BrightColor;

in vec3 Color;
in float Alpha;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform vec3 viewPos;

void main() {
    // Distance from fragment to black hole for distortion effects
    float dist = length(gl_FragCoord.xyz - vec3(blackHolePos.xy, 0.0));
    
    // Base color with transparency
    vec3 finalColor = Color;
    float finalAlpha = Alpha;
    
    // Add subtle glow effect near boundaries
    float glowStrength = 0.0;
    
    // Check if near important boundaries
    float photonSphere = schwarzschildRadius * 1.5f;
    float isco = schwarzschildRadius * 3.0f;
    
    // Add pulsing effect to make boundaries more visible
    float pulse = sin(gl_FragCoord.x * 0.01 + gl_FragCoord.y * 0.01) * 0.1 + 0.9;
    
    // Special highlighting for critical boundaries
    if (abs(dist - schwarzschildRadius) < 0.1f) {
        // Event horizon - red glow
        glowStrength = 0.8f;
        finalColor = mix(finalColor, vec3(1.0f, 0.2f, 0.2f), 0.6f);
    } else if (abs(dist - photonSphere) < 0.1f) {
        // Photon sphere - orange glow
        glowStrength = 0.6f;
        finalColor = mix(finalColor, vec3(1.0f, 0.6f, 0.2f), 0.4f);
    } else if (abs(dist - isco) < 0.1f) {
        // Innermost stable circular orbit - yellow glow
        glowStrength = 0.4f;
        finalColor = mix(finalColor, vec3(1.0f, 0.8f, 0.3f), 0.3f);
    }
    
    // Apply glow
    finalColor += glowStrength * pulse * vec3(0.3f, 0.4f, 0.6f);
    finalAlpha = min(finalAlpha + glowStrength * 0.3f, 1.0f);
    
    FragColor = vec4(finalColor, finalAlpha);
    
    // Bright color for bloom on important boundaries
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if (glowStrength > 0.3f) {
        BrightColor = vec4(finalColor * 2.0f, finalAlpha);
    } else {
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}