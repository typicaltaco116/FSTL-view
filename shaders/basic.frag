#version 330 core
in float fragmentDistance;

out vec4 FragColor;

const float period = 1.0f;
const float PI = 3.14159265359;
const vec3 phases = radians(vec3(0.0f, 90.0f, 180.0f));

void main()
{
	vec3 colors = 0.5f * (cos(2.0f*PI*(1.0f/period)*vec3(fragmentDistance) - phases) + 1.0f);
	FragColor = vec4(colors, 1.0f);
}
