#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 col;

layout (location = 4) uniform mat4 cam;
layout (location = 5) uniform mat4 invCam;

flat out vec3 normal;
flat out vec3 color;
out vec3 fragPos;

/*
const mat4 scale = mat4(
	-1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, -1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
	);
*/
void main(){
	color = col;
	normal = nrm;
	//mat4 trans = cam*(scale);
	fragPos = (gl_Position = cam*vec4(pos, 1.0)).xyz;
}