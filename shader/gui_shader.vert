#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv_;
layout (location = 2) in vec4 col_;

layout (location = 3) uniform mat4 cam;

out vec2 uv;
out vec4 col;

void main() {
    uv = uv_;
    col = col_;
    gl_Position = cam * vec4(pos.xy, 0, 1);
}