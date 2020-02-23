#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float model[16];
uniform float perspective[16];

out vec3 myColor;

void main() {
    float x = (model[0] * aPos.x) + (model[1] * aPos.y) + (model[2] * aPos.z) + model[3];
    float y = (model[4] * aPos.x) + (model[5] * aPos.y) + (model[6] * aPos.z) + model[7];
    float z = (model[8] * aPos.x) + (model[9] * aPos.y) + (model[10] * aPos.z) + model[11];
    float w = (model[12] * aPos.x) + (model[13] * aPos.y) + (model[14] * aPos.z) + model[15];

    float x1 = (perspective[0] * x) + (perspective[1] * y) + (perspective[2] * z) + (perspective[3] * w);
    float y1 = (perspective[4] * x) + (perspective[5] * y) + (perspective[6] * z) + (perspective[7] * w);
    float z1 = (perspective[8] * x) + (perspective[9] * y) + (perspective[10] * z) + (perspective[11] * w);
    float w1 = (perspective[12] * x) + (perspective[13] * y) + (perspective[14] * z) + (perspective[15] * w);
    
    gl_Position = vec4(x1, y1, z1, w1);
    myColor = aColor;
}
