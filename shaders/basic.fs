#version 330 core

in vec3 fragmentPos;
in vec3 normal;

uniform vec3 lightPos;

out vec4 FragColor;

void main()
{
    vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightDir = normalize(lightPos - fragmentPos);
    float intensity = max(dot(normal, lightDir), 0.0f);

	FragColor = vec4(baseColor * intensity, 1.0f);
}
