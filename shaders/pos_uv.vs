#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 out_uv;

uniform mat4 mvp;

void main(){
	
	gl_Position = mvp * vec4(pos, 1);
	
	out_uv = uv;
}
