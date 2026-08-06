#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 globalPos[];

out vec3 fragmentPos;
out vec3 normal;

void main() {
    vec3 edge1 = globalPos[1] - globalPos[0];
    vec3 edge2 = globalPos[2] - globalPos[0];

    vec3 norm = normalize(cross(edge1, edge2));

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position; // pass through unchanged.
        fragmentPos = globalPos[i];
        normal = norm;
        EmitVertex();
    }

    EndPrimitive();
}
