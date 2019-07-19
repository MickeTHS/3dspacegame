#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

out vec3 out_color;

uniform mat4 mvp;

void main(){
	
	gl_Position = mvp * vec4(pos, 1);
	
	// UV of the vertex. No special space for this one.
	out_color = col;
}
