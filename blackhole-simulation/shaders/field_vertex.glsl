#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aAlpha;

out vec3 Color;
out float Alpha;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Color = aColor;
    Alpha = aAlpha;
    gl_Position = projection * view * vec4(aPos, 1.0);
}