#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 transMat;

out float fragmentDistance;
out vec3 norm;

void main()
{
	fragmentDistance = length(aPos);
    norm = aNorm;
	gl_Position = transMat * vec4(aPos, 1.0);
}
