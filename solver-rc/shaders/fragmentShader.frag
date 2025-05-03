#version 460 core

in vec4 vertexColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(
        vertexColor.x,
        vertexColor.y,
        vertexColor.z,
        1.0
    );
}