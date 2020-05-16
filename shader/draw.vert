#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec4 col;

layout(location = 3) uniform mat4 cam;

out vec4 vertexColor;

void main() {
    gl_Position = cam * vec4(pos, 1.0);
    vertexColor = vec4(col.xyza);
}