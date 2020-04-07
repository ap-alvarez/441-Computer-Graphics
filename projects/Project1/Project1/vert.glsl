#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat3 itModel;
uniform mat4 projection;
uniform mat4 camera;

out vec2 uv;
out vec3 ourNormal;
out vec3 ourFragPos;

void main() {
    gl_Position = projection * camera * model * vec4(aPos, 1.0);
    ourNormal = itModel * aNormal;
    ourFragPos = vec3(model * vec4(aPos, 1.0));
    uv = aUV;
}