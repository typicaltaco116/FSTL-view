#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 dir;
uniform float theta;

void main()
{
	float Rx = dir.x;
	float Ry = dir.y;
	float Rz = dir.z;
	mat4 rotMat = mat4(
		cos(theta)+Rx*Rx*(1-cos(theta)), Rx*Ry*(1-cos(theta))-Rz*sin(theta), Rx*Ry*(1-cos(theta))+Ry*sin(theta), 0,
		Ry*Rx*(1-cos(theta))+Rz*sin(theta), cos(theta)+Ry*Ry*(1-cos(theta)), Ry*Rz*(1-cos(theta))-Rx*sin(theta), 0,
		Rz*Rx*(1-cos(theta))-Ry*sin(theta), Rz*Ry*(1-cos(theta))+Rx*sin(theta), cos(theta)+Rz*Rz*(1-cos(theta)), 0,
		0, 0, 0, 1
	);
	gl_Position = rotMat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
