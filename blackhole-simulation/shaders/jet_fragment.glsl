#version 430 core
out vec4 FragColor;
out vec4 BrightColor;

in vec3 FragPos;
in vec3 Color;
in float Size;
in float Intensity;
in float Temperature;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform vec3 viewPos;

// Noise function for plasma turbulence
float noise(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

void main() {
    // Create circular particle shape
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    // Soft edges for particles
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
    
    // Add some noise for plasma effect
    float plasmaNoise = noise(FragPos * 2.0) * 0.3 + 0.7;
    
    // Distance from black hole affects color
    float distToBlackHole = length(FragPos - blackHolePos);
    float gravitationalEffect = 1.0 + schwarzschildRadius / (distToBlackHole * distToBlackHole);
    gravitationalEffect = clamp(gravitationalEffect, 1.0, 3.0);
    
    // Doppler effect based on motion relative to viewer
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 motionDir = normalize(vec3(FragPos.x, 0.0, FragPos.z)); // Radial motion
    float doppler = dot(viewDir, motionDir);
    
    // Base color with temperature influence
    vec3 plasmaColor = Color;
    
    // Apply Doppler shifting
    if (doppler > 0.0) {
        // Moving toward viewer - blue shift
        plasmaColor.r *= (1.0 - doppler * 0.2);
        plasmaColor.b *= (1.0 + doppler * 0.3);
    } else {
        // Moving away - red shift
        plasmaColor.r *= (1.0 - doppler * 0.3);
        plasmaColor.b *= (1.0 + doppler * 0.2);
    }
    
    // Apply gravitational blueshift for particles close to black hole
    if (distToBlackHole < schwarzschildRadius * 5.0) {
        float blueshift = 1.0 - (schwarzschildRadius * 2.0) / distToBlackHole;
        blueshift = clamp(blueshift, 0.0, 1.0);
        plasmaColor = mix(plasmaColor, vec3(0.7, 0.8, 1.0), 1.0 - blueshift);
    }
    
    // Intensity based on temperature, lifetime, and distance
    float finalIntensity = Intensity * Temperature * plasmaNoise;
    
    // Add glow effect for hot particles
    float glow = 0.0;
    if (Temperature > 0.7) {
        glow = (Temperature - 0.7) * 3.0;
    }
    
    // Final color
    vec3 finalColor = plasmaColor * finalIntensity * gravitationalEffect;
    finalColor += vec3(0.3, 0.4, 0.8) * glow; // Bluish glow
    
    // Add some scattering effect
    finalColor += vec3(0.1, 0.1, 0.2) * alpha * 0.3;
    
    // Apply alpha
    float finalAlpha = alpha * finalIntensity * 0.8;
    finalAlpha = clamp(finalAlpha, 0.0, 1.0);
    
    FragColor = vec4(finalColor, finalAlpha);
    
    // Bright color for bloom effect
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.5 || Temperature > 0.8) {
        BrightColor = vec4(finalColor * 2.0, finalAlpha);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}