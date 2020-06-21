
#version 430 core

const float PI =  3.14159265358979323846;

out float fragColor;
  
layout (location = 2) uniform sampler2D g_pos;
layout (location = 3) uniform sampler2D g_nrm;
layout (location = 4) uniform sampler2D g_t;
layout (location = 5) uniform sampler2D g_bt;

layout (location = 7) uniform sampler2D g_noise;

layout(location = 8) uniform mat4 projection;
layout(location = 9) uniform vec2 bounds;
layout(location = 10) uniform float radius = 1.f;
layout(location = 11) uniform float bias = 0.025;
layout (location = 12) uniform mat4 view;
layout(location = 13) uniform float samples[3*64];

in vec2 uvs;

const int kernelSize = 64;


void main() {

    const vec2 noiseScale = bounds / 4.f;

    const vec3 pos = vec3((view*texture(g_pos, uvs)).xyz);
    const vec3 N = texture(g_nrm, uvs).xyz;   
    const vec3 rand = normalize(texture(g_noise, uvs * noiseScale).xyz);

    const vec3 T = normalize(rand - N * dot(rand, N));
    const vec3 BT = cross(N, T);
    const mat3 TBN = mat3(T, BT, N);

    float occlusion = 0.f;
    for(int i = 0; i < kernelSize; ++i) {
        // get sample position
        vec3 samp = TBN * vec3(samples[3*i], samples[3*i+1], samples[3*i+2]); // from tangent to view-space
        samp = pos + samp * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samp, 1.f);
        offset = projection * offset; // from view to clip-space
        offset.xy /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = (view*texture(g_pos, offset.xy)).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(pos.z - sampleDepth));
        occlusion += (sampleDepth >= samp.z + bias ? 1.f : 0.f) * rangeCheck;           
    }
    occlusion = 1.f - (occlusion / kernelSize);
    
    fragColor = occlusion;
}
