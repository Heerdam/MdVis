#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 uvs;

void main() {
    gl_Position = vec4(position.xy, 0.f, 1.f);
    uvs = uv;
}