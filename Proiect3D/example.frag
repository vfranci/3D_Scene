#version 330 core

in vec2 TexCoord;  // Coordonatele texturii primite de la vertex shader

out vec4 FragColor; // Culorile finale ale pixelilor

uniform sampler2D texture1; // Textura de aplicat

void main() {
    FragColor = texture(texture1, TexCoord); // Aplică textura
}
