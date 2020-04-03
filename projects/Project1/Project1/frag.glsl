#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform sampler2D ourTexture;

void main() {
    fragColor = texture(ourTexture, uv);
}
