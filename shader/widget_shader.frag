#version 430 core

out vec4 fragColor;

flat in vec3 camP;
in vec3 fragPos;
flat in vec3 normal;
flat in vec3 color;

layout (location = 3) uniform vec3 camPos;

const vec3 light = vec3(100.f, 100.f, 100.f);

void main() {
    // ambient
    //fragColor = vec4(normal, 1.f);
    //return;
    float ambientStrength = 0.4;
    vec3 ambient = vec3(1.f, 1.f, 1.f) * ambientStrength;
  	
    // diffuse 
    vec3 lightDir = normalize(light - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = vec3(1.f, 1.f, 1.f) * diff;
    fragColor = vec4((ambient + diffuse) * color, 1.0);
}