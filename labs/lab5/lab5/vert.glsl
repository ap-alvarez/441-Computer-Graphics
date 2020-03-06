#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 camera;

out vec3 ourColor;
out vec3 ourNormal;
out vec3 ourPos;

void main() {
    gl_Position = projection * camera * model * vec4(aPos, 1.0);
    ourColor = aColor;
    ourNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    ourPos = (model * vec4(aPos, 1.0)).xyz;
}
