
#version 460 core

const float PI =  3.14159265358979323846;

//out vec4 fragColor;
 out vec4 fragColor;
  
layout (location = 2) uniform sampler2D g_pos;
layout (location = 3) uniform sampler2D g_nrm;
layout (location = 4) uniform sampler2D g_t;
layout (location = 5) uniform sampler2D g_bt; 
layout (location = 6) uniform sampler2D g_col;
layout (location = 7) uniform sampler2D g_ssao;

layout(location = 8) uniform vec3 camPos;

layout (location = 9) uniform float ambiente = 0.1f;

layout (location = 10) uniform float intensity = 0.5f;

layout(location = 12) uniform float lights[4*6];

in vec2 uvs;

void main() {

    vec3 pos = texture(g_pos, uvs).xyz;
    vec3 V = -normalize(pos - camPos.xyz);
    vec3 N = texture(g_nrm, uvs).xyz;
    vec3 albedo = texture(g_col, uvs).rgb;
    float ao = texture(g_ssao, uvs).x;// * 2 - 1;

    //fragColor = vec3(ao, ao, ao);
    //return;

    vec3 light = vec3(1.f) * ambiente * ao;

    for(uint i = 0; i < 4; ++i) {
        vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);

        vec3 L = normalize(lightPos - pos);
        float d = max(dot(N, L), 0.f);
        float diff = d < 0.5f ? d*d :d - 0.25f ;
        vec3 diffuse =  diff * albedo * (intensity * lightCol);
        light += diffuse;
    }

   fragColor = vec4(light.xyz, 1.0);
   //fragColor = light;
}













/*

const uint samples = 4;
const float radius = 50.f;
const float frac = radius/samples;


void main() {
    vec3 pos = texture(g_pos, uvs).xyz;
    vec3 V = -normalize(pos);//-normalize(pos - camPos.xyz);
    vec3 N = texture(g_nrm, uvs).xyz;
    vec3 albedo = texture(g_col, uvs).rgb;
    float ao = texture(g_ssao, uvs).r;

    vec3 res = vec3(0.f);
    //dir light
    float diff = max(dot(N, dirLight), 0.0);
    // combine results
    
  	vec3 diffuse = vec3(0.f);
    for(uint i = 0; i < 4; ++i) {
        const vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        const vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);

        // diffuse
        vec3 lightDir = normalize(lightPos - pos);
        vec3 tang = normalize(cross(lightDir, vec3(0.f, 1.f, 0.f)));
        vec3 btang = normalize(cross(lightDir, tang));

        float l = 0.f;
        for(int i = 0; i < samples; ++i){
            for(int j = 0; j < samples; ++j){
                float e1 = -radius/2 + frac*i;
                float e2 = -radius/2 + frac*j;
                vec3 nlp = lightPos + tang * e1 + btang * e2;
                vec3 lightDir = normalize(nlp - pos); 
                float d = dot(N, lightDir);
                float diff = d < 0.5f ? d*d : d;
                l += diff;
            }
        }       
        l /= samples;

        diffuse += l * (intensity*lightCol) * albedo;
    }

    vec3 ambient = ambiente * vec3(1.f)*intensity;
    fragColor = vec4(ambient + diff*vec3(1.f)*intensity + diffuse, 1.f);
}














const float sigma = 0.3f;
const float sigma2 = sigma * sigma;
const float A = 1.f - 0.5*(sigma2)/(sigma2 + 0.33f);
const float B = 0.45f * sigma2 / (sigma2 + 0.09f);



void main() {

    vec3 pos = texture(g_pos, uvs).xyz;
    vec3 V = -normalize(pos);//-normalize(pos - camPos.xyz);
    vec3 N = texture(g_nrm, uvs).xyz;
    vec3 albedo = texture(g_col, uvs).rgb;
    float ao = texture(g_ssao, uvs).r;

    // ambient
   
  	vec3 diffuse = vec3(0.f);
    for(uint i = 0; i < 4; ++i) {
        const vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        const vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);

        // diffuse
        vec3 lightDir = normalize(lightPos - pos);
        vec3 tang = normalize(cross(lightDir, vec3(0.f, 1.f, 0.f)));
        vec3 btang = normalize(cross(lightDir, tang));

        float l = 0.f;
        for(int i = 0; i < samples; ++i){
            for(int j = 0; j < samples; ++j){
                float e1 = -radius/2 + frac*i;
                float e2 = -radius/2 + frac*j;
                vec3 nlp = lightPos + tang * e1 + btang * e2;
                vec3 lightDir = normalize(nlp - pos);              
                float diff = max(dot(N, lightDir), 0.1);
                l += diff;
            }
        }       
        l /= samples;
        diffuse += l * (intensity*lightCol) * albedo;
    }

    fragColor = vec4(ambient + diffuse, 1.0);
    return;


     
    //fragColor = vec4(ao, ao, ao, 1.f);

    vec3 light = ambiente * vec3(1.f);// * ao;
   
    for(uint i = 0; i < 4; ++i) {
        const vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        const vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);
        
        vec3 lDir = normalize(lightPos - pos);
        
        const float LdotV = dot(lDir, V);
        const float NdotL = dot(lDir, N);
        const float NdotV = dot(N, V);
        
        const float s = LdotV - NdotL * NdotV;
        const float t = mix(1.f, max(NdotL, NdotV), step(0.f, s));

        const float res = max(0.f, NdotL) * (A + B * s / t) / PI;
        
        light += clamp(albedo * res*lightCol, 0.f, 1.f);
    }
    fragColor = vec4(light.xyz, 1.0);
    return;
    
    

   
   for(uint i = 0; i < 4; ++i) {
        vec3 lightPos = vec3(lights[6*i], lights[6*i+1], lights[6*i+2]);
        vec3 lightCol = vec3(lights[6*i+3], lights[6*i+4], lights[6*i+5]);

        vec3 L = normalize(lightPos - pos);

        vec3 diffuse =  max(dot(N, L), 0.0) * albedo * (intensity * lightCol);
        light += diffuse;
    }

    fragColor = vec4(light.xyz, 1.0);
    //fragColor.rgb = pow(fragColor.rgb, vec3(1.f/2.2f));
    
    
}
*/