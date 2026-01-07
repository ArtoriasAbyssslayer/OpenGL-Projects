#version 430 core
out vec4 FragColor;
out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 blackHolePos;
uniform float schwarzschildRadius;
uniform float innerRadius;
uniform float outerRadius;
uniform vec3 viewPos;
uniform float time; // Add time uniform for animation

// Noise function for turbulence
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for(int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

void main() {
    // Distance from fragment to black hole center
    float dist = length(FragPos - blackHolePos);
    
    // Distance along the disk (radial distance)
    float radialDist = length(vec2(FragPos.x, FragPos.z) - vec2(blackHolePos.x, blackHolePos.z));
    
    // Angular position for spiral structure
    float angle = atan(FragPos.z - blackHolePos.z, FragPos.x - blackHolePos.x);
    
    // Temperature gradient (hotter near black hole)
    float temperature = 1.0 - smoothstep(innerRadius, outerRadius, radialDist);
    
    // Enhanced spiral structure with more prominent arms
    float spiral1 = sin(angle * 2.0 - radialDist * 1.5 + time * 1.5);
    float spiral2 = sin(angle * 3.0 + radialDist * 2.0 - time * 1.0);
    float spiral3 = sin(angle * 4.0 - radialDist * 1.8 + time * 0.8);
    
    // Create distinct spiral arms
    float spiralPattern = (spiral1 * 0.6 + spiral2 * 0.3 + spiral3 * 0.1) * 0.5 + 0.5;
    
    // Add dust lanes between spiral arms
    float dustLane1 = pow(sin(angle * 2.0 - radialDist * 1.2), 2.0);
    float dustLane2 = pow(sin(angle * 3.0 + radialDist * 1.8), 3.0);
    float dustPattern = (dustLane1 * 0.6 + dustLane2 * 0.4);
    
    // Combine spiral and dust patterns
    float combinedStructure = spiralPattern * (1.0 - dustPattern * 0.7);
    
    // Turbulence using fractal Brownian motion
    vec2 uv = vec2(radialDist * 0.1, angle * 0.5 + time * 0.1);
    float turbulence = fbm(uv);
    
    // Combine enhanced spiral structure with turbulence
    float structure = combinedStructure * (0.8 + 0.2 * turbulence);
    
    // Orbital velocity effect (faster near black hole, following Kepler's laws)
    float velocity = sqrt(schwarzschildRadius / max(radialDist, innerRadius * 1.1));
    
    // Enhanced temperature based on structure (brighter in spiral arms)
    temperature *= (0.7 + 0.6 * structure);
    
    // Doppler shifting based on velocity
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 tangentDir = normalize(vec3(-FragPos.z, 0.0, FragPos.x));
    float dopplerFactor = dot(viewDir, tangentDir) * velocity;
    
    // Blackbody radiation based on temperature
    // Using simplified blackbody color approximation
    vec3 blackbodyColor;
    if (temperature > 0.8) {
        // Very hot - blue-white
        blackbodyColor = vec3(0.9, 0.95, 1.0);
    } else if (temperature > 0.6) {
        // Hot - white-yellow
        blackbodyColor = vec3(1.0, 0.95, 0.8);
    } else if (temperature > 0.4) {
        // Medium - orange-yellow
        blackbodyColor = vec3(1.0, 0.7, 0.3);
    } else {
        // Cool - red
        blackbodyColor = vec3(0.8, 0.3, 0.1);
    }
    
    // Apply structure-based color variation
    vec3 structureColor = mix(vec3(1.0, 0.5, 0.2), vec3(0.5, 0.8, 1.0), structure);
    vec3 baseColor = mix(blackbodyColor, structureColor, 0.3);
    
    // Enhanced Doppler shifting
    dopplerFactor *= 2.0;
    vec3 dopplerColor = baseColor;
    if (dopplerFactor > 0.0) {
        // Blue shift
        dopplerColor.r *= (1.0 - dopplerFactor * 0.4);
        dopplerColor.b *= (1.0 + dopplerFactor * 0.3);
    } else {
        // Red shift
        dopplerColor.r *= (1.0 - dopplerFactor * 0.3);
        dopplerColor.b *= (1.0 + dopplerFactor * 0.4);
    }
    
    // Intensity variation based on turbulence and enhanced structure
    float intensity = 1.0 + 0.6 * turbulence + 0.5 * structure;
    
    // Realistic glow with inverse square falloff
    float glowIntensity = temperature * intensity * velocity * 3.0;
    glowIntensity *= (1.0 + 1.0 / (radialDist * radialDist * 0.01));
    
    // Add edge glow for outer disk regions
    float edgeGlow = smoothstep(outerRadius * 0.8, outerRadius, radialDist) * 
                     (1.0 - smoothstep(outerRadius * 0.8, outerRadius, radialDist));
    
    // Combine all effects
    vec3 finalColor = dopplerColor * intensity;
    finalColor += vec3(0.2, 0.1, 0.05) * edgeGlow; // Reddish edge glow
    finalColor *= glowIntensity;
    
    // Add subtle emission from hot plasma
    finalColor += baseColor * temperature * 0.3;
    
    // Varying transparency based on temperature and structure
    float alpha = (temperature * 0.7 + 0.1) * (0.8 + 0.2 * structure);
    alpha = clamp(alpha, 0.1, 0.9);
    
    // Final HDR output
    FragColor = vec4(finalColor, alpha);
    
    // Bright color for bloom effect (threshold based on temperature)
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 2.0 || temperature > 0.7) {
        BrightColor = vec4(finalColor * 1.5, alpha);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
    
    // Add glow effect
    float glowIntensity = temperature * velocity * 2.0;
    vec3 glowColor = baseColor * glowIntensity;
    
    // Final color with transparency
    vec3 finalColor = baseColor + glowColor;
    float alpha = temperature * 0.8 + 0.2;
    
    FragColor = vec4(finalColor, alpha);
}