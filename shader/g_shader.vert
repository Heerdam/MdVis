#version 460 core

layout (location = 5) in vec3 position;
layout (location = 6) in vec3 nrm;
layout (location = 7) in vec3 t;
layout (location = 8) in vec3 bt;

layout (location = 9) uniform vec4 col;
layout (location = 10) uniform mat4 cam;


out vec3 pos;
out vec4 vertexColor;
out vec3 N;
out vec3 X;
out vec3 Y;

void main() {
    gl_Position = cam * vec4(position, 1.f);
    pos = vec4(position, 1.f).xyz;
    N = nrm;
    X = t;
    Y = bt;
    vertexColor = col;
}