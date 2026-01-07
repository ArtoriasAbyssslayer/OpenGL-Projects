#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;
layout (location = 3) in float aIntensity;
layout (location = 4) in float aTemperature;

out vec3 FragPos;
out vec3 Color;
out float Size;
out float Intensity;
out float Temperature;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Color = aColor;
    Size = aSize;
    Intensity = aIntensity;
    Temperature = aTemperature;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    gl_PointSize = Size * 3.0;
}