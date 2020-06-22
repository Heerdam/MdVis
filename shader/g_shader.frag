
#version 430 core

layout(location = 0) out vec4 g_pos;
layout(location = 1) out vec4 g_nrm;
layout(location = 2) out vec4 g_t;
layout(location = 3) out vec4 g_bt;
layout(location = 4) out vec4 g_col;

in vec4 vertexColor; 

in vec3 pos;
in vec3 N;
in vec3 X; //tangent
in vec3 Y; //bitangent

void main() {
    //float d = 2.0 - gl_FragCoord.z - 1.f;
    g_pos = vec4(pos, 0.f);
    g_nrm = vec4(N, 0.f);
    g_t = vec4(X, 0.f);
    g_bt = vec4(Y, 0.f);
    g_col = vertexColor;
}
