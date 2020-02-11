#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform float transform[9];

out vec3 myColor;

void main() {
    float x = (transform[0] * aPos.x) + (transform[1] * aPos.y) + transform[2];
    float y = (transform[3] * aPos.x) + (transform[4] * aPos.y) + transform[5];
    gl_Position = vec4(x, y, 0.0, 1.0);
    myColor = aColor;
}
