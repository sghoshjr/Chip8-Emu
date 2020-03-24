#version 330 core

layout(location = 0) in vec3 vertexPos_M;

out vec2 UV;

void main() {
    UV = vec2((vertexPos_M.x + 1)/2., 1 - (vertexPos_M.y + 1)/2.);
    gl_Position = vec4(vertexPos_M, 1.0);
}