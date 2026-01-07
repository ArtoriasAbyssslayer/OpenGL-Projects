#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in float Brightness;
in float Size;
in vec3 Color;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform vec3 viewPos;

void main() {
    // Distance from star to black hole
    float distToBlackHole = length(FragPos - blackHolePos);
    
    // Gravitational lensing effect on stars
    float lensingStrength = schwarzschildRadius / (distToBlackHole * distToBlackHole);
    lensingStrength = clamp(lensingStrength, 0.0, 0.8);
    
    // Distance from viewer to star
    float distToViewer = length(viewPos - FragPos);
    
    // Star brightness with distance attenuation
    float attenuation = Brightness / (1.0 + distToViewer * 0.01);
    
    // Apply lensing brightening effect
    float lensingBrightness = 1.0 + lensingStrength * 2.0;
    
    // Final star color
    vec3 finalColor = Color * attenuation * lensingBrightness;
    
    // Add subtle twinkling effect based on position
    float twinkle = sin(gl_FragCoord.x * 0.1 + gl_FragCoord.y * 0.1) * 0.1 + 0.9;
    finalColor *= twinkle;
    
    // Alpha based on brightness
    float alpha = clamp(attenuation * lensingBrightness, 0.3, 1.0);
    
    FragColor = vec4(finalColor, alpha);
}