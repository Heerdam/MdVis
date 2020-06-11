#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) uniform mat4 cam;
layout (location = 2) uniform mat4 model;

void main(){
	 gl_Position = cam*(model*vec4(pos, 1.0));
}