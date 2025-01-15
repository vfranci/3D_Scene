#version 330 core

layout (location = 0) in vec3 aPos;  // Pozițiile vertecilor
layout (location = 1) in vec2 aTexCoord; // Coordonatele texturii

out vec2 TexCoord;  // Ieșirea coordonatelor texturii

uniform mat4 model;   // Matricea modelului
uniform mat4 view;    // Matricea vederii
uniform mat4 projection; // Matricea proiecției

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);  // Calculul poziției finale
    TexCoord = aTexCoord;  // Transmiterea coordonatelor texturii
}
