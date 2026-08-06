#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out float fragmentDistance;
out vec3 norm;

void main()
{
	fragmentDistance = length(aPos);
    norm = aNorm;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
