
#version 460 core

const float PI =  3.14159265358979323846;

out vec4 fragColor;
  
layout (location = 2) uniform sampler2D g_pos;
layout (location = 3) uniform sampler2D g_nrm;
layout (location = 4) uniform sampler2D g_t;
layout (location = 5) uniform sampler2D g_bt; 
layout (location = 6) uniform sampler2D g_col;

layout(location = 7) uniform vec3 camPos;

layout (location = 8) uniform float ambiente = 0.1f;

layout(location = 9) uniform float lights[4*6];

in vec2 uvs;

void main() {

    vec3 pos = texture(g_pos, uvs).xyz;
    vec3 V = -normalize(pos - camPos.xyz);
    vec3 N = texture(g_nrm, uvs).xyz;
    vec3 albedo = texture(g_col, uvs).rgb;

    vec3 light = albedo * ambiente;

    for(uint i = 0; i < 4; ++i) {
        vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);

        vec3 L = normalize(lightPos - pos);

        vec3 diffuse =  max(dot(N, L), 0.0) * albedo * lightCol;
        light += diffuse;
    }

    fragColor = vec4(light.xyz, 1.0);
    
}
