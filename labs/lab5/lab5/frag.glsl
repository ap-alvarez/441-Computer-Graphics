#version 330 core
in vec3 ourColor;
in vec3 ourNormal;

out vec4 fragColor;

uniform vec4 lightPos;
uniform vec4 lightColor;
//uniform float lightIntensity;
uniform vec4 ambient;
uniform vec4 camPos;
uniform mat4 projection;
uniform mat4 camera;

void main() {
    vec3 gazeDir = -(camPos.xyz);
    vec3 diffColor = ourColor * (ambient.xyz + (lightColor.xyz * max(0,dot(ourNormal, lightPos.xyz))));
    vec3 phongColor = lightColor.xyz *  pow(  max( 0, dot(gazeDir, gazeDir - (2 * dot(gazeDir, ourNormal) * ourNormal))), 4.0f);

    fragColor = clamp(vec4(diffColor + phongColor, 1.0f), vec4(0.0f,0.0f,0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

