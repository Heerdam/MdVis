
#version 460 core

//out vec4 frag;
layout(location = 0) out vec3 g_pos;
layout(location = 1) out vec3 g_nrm;
layout(location = 2) out vec3 g_t;
layout(location = 3) out vec3 g_bt;
layout(location = 4) out vec3 g_col;

in vec4 vertexColor; 

in vec3 pos;
in vec3 N;
in vec3 X; //tangent
in vec3 Y; //bitangent

void main() {
    g_pos = pos;
    g_pos = vec3(0.5f, 0.5f, 0.5f);
    g_nrm = N;
    g_t = X;
    g_bt = Y;
    g_col = vertexColor.xyz;
    //frag = vec4(0.5f, 0.5f, 0.5f, 1.f);
}
