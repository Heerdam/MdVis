#version 460 core

out vec4 fragColor;

in vec2 uv;
in vec4 col;

layout (location = 4) uniform sampler2D Texture;

void main() {
    fragColor = col * texture(Texture, uv);
    //fragColor = vec4(0.5f, 0.5f, 0.5f, 1.f);
}
