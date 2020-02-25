#version 330 core
in vec3 ourColor;
in vec3 ourNormal;

out vec4 fragColor;

uniform vec4 lightPos;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec4 gazeDir;
uniform float specStrength;

void main() {
    vec3 lightDir = normalize(lightPos.xyz);
    float diff = max(0.0, dot(ourNormal, lightDir));
    vec3 diffuse = diff * vec3(lightColor);
    vec3 diffColor = ourColor * (ambient.xyz + diffuse);

    vec3 reflect = reflect(-lightDir, ourNormal);
    float spec = pow(max(0,dot(normalize(gazeDir.xyz), reflect)), 8);
    vec3 specular = specStrength * spec * lightColor.xyz;

    fragColor = clamp(vec4(diffColor + specular, 1.0f), vec4(0.0f,0.0f,0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

